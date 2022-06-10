#include "global_transformers.hpp"
#include "cfg_cleaner.hpp"
#include <set>

// TailrecEliminator

TailrecEliminator::TailrecEliminator(IntermediateUnit const &unit, CFGraph rawCfg, int funcId)
        : IRTransformer(std::move(rawCfg)), iunit(&unit) {
    passFunction(funcId);

    CfgCleaner cleaner(unit, std::move(cfg));
    cleaner.removeNops();
    cleaner.removeUselessNodes();
    cfg = std::move(cleaner).moveCfg();
}

void TailrecEliminator::passFunction(int funcId) {
    std::vector<int> tailrecBlocks = findTailCalls(funcId);
    if (tailrecBlocks.empty())
        return;

    IR_Block &oldHead = cfg.block(cfg.entryBlockId);

    // NewHead is a dummy block, created because entry block cannot have predcessors
    IR_Block &newHead = cfg.createBlock();
    newHead.setTerminator(IR_ExprTerminator::JUMP);
    cfg.linkBlocks(newHead, oldHead);
    cfg.entryBlockId = newHead.id;

    std::vector<IRval> newArgs;
    std::vector<IR_ExprPhi *> phis;
    auto const &argsTypes = iunit->getFunction(funcId).getFuncType()->args;

    // Place phis in oldHead (there are no other phis because it was entry block)
    for (uint64_t an = 0; auto const &argType : argsTypes) {
        IRval arg = cfg.createReg(argType);
        newArgs.push_back(arg);

        auto phi = std::make_unique<IR_ExprPhi>();
        phi->args.emplace(0, IRval::createFunArg(argType, an));
        phis.push_back(phi.get());
        oldHead.phis.emplace_back(arg, std::move(phi));

        an++;
    }

    // Remove tail calls
    for (int i = 1; int tailId : tailrecBlocks) {
        IR_Block &tailBlock = cfg.block(tailId);
        tailBlock.setTerminator(IR_ExprTerminator::JUMP);
        cfg.linkBlocks(tailBlock, oldHead);

        auto &lastNode = tailBlock.body.back();
        auto const &callExpr = dynamic_cast<IR_ExprCall const &>(*lastNode.body);
        for (uint64_t an = 0; an < argsTypes.size(); an++) {
            phis[an]->args.emplace(i, callExpr.args[an].copy());
        }
        lastNode = IR_Node::nop();

        i++;
    }

    replaceParams(cfg.entryBlockId, newArgs);
}

std::vector<int> TailrecEliminator::findTailCalls(int funcId) {
    std::set<int> visited;
    std::vector<int> tailrecBlocks;

    auto visitor = [this, funcId, &tailrecBlocks](int blockId) {
        IR_Block &curBlock = cfg.block(blockId);

        // Inspect only blocks with return statement
        auto terminator = curBlock.getTerminator();
        if (terminator->termType != IR_ExprTerminator::RET)
            return;

        if (curBlock.body.empty())
            return;

        // TODO: check if can reorder
        auto const &lastNode = curBlock.body.back();
        if (!lastNode.body || lastNode.body->type != IR_Expr::CALL)
            return;

        auto const &callExpr = dynamic_cast<IR_ExprCall const &>(*lastNode.body);
        if (callExpr.isIndirect() || callExpr.getFuncId() != funcId)
            return;

        // Check if returns result of recursive call or nothing
        if (terminator->arg.has_value()) {
            if (!lastNode.res.has_value()) // TODO: also can be tailcall
                return;
            if (*lastNode.res != *terminator->arg)
                return;
        }

        tailrecBlocks.push_back(blockId);
    };

    cfg.traverseBlocks(cfg.entryBlockId, visited, visitor);
    return tailrecBlocks;
}

void TailrecEliminator::replaceParams(int entryId, std::vector<IRval> const &newArgs) {
    std::set<int> visited;
    auto visitor = [this, &newArgs](int blockId) {
        IR_Block &block = cfg.block(blockId);
        for (IR_Node &node : block.body) {
            if (node.body) {
                for (auto arg: node.body->getArgs()) {
                    if (arg->getValueClass() == IRval::FUN_PARAM)
                        *arg = newArgs.at(arg->castValTo<int>());
                }
            }
        }
    };
    cfg.traverseBlocks(entryId, visited, visitor);
}

// Inliner

FunctionsInliner::FunctionsInliner(IntermediateUnit const &unit, CFGraph rawCfg)
        : IRTransformer(std::move(rawCfg)), iunit(&unit) {
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto &[blockId, block] : cfg.getBlocksData()) {
            if (passBlock(block)) {
                changed = true;
                break;
            }
        }
    }

    CfgCleaner cleaner(unit, std::move(cfg));
    cleaner.removeTransitBlocks();
    cleaner.removeNops();
    cfg = std::move(cleaner).moveCfg();
}

bool FunctionsInliner::passBlock(IR_Block &block) {
    for (auto nodeIt = block.body.begin(); nodeIt != block.body.end(); ++nodeIt) {
        IR_Node &node = *nodeIt;
        if (node.body && node.body->type == IR_Expr::CALL) {
            auto const &callExpr = node.body->getCall();
            if (callExpr.isIndirect())
                continue;
            auto const &func = iunit->getFunction(callExpr.getFuncId());
            if (func.isInline()) {
                IR_Block &retBlock = cfg.createBlock();
                IR_Block &entry = inlineFunc(func, retBlock, node);

                IR_Block &remBlock = cfg.createBlock();
                retBlock.setTerminator(IR_ExprTerminator::JUMP);
                retBlock.next = { remBlock.id };
                remBlock.prev = { retBlock.id };

                // Move remaining instructions
                *nodeIt = IR_Node::nop();
                ++nodeIt;
                while (nodeIt != block.body.end()) {
                    remBlock.body.push_back(std::exchange(*nodeIt, IR_Node::nop()));
                    ++nodeIt;
                }

                remBlock.termNode = std::move(block.termNode);
                remBlock.next = std::move(block.next);
                block.setTerminator(IR_ExprTerminator::JUMP);
                block.next = { entry.id };
                entry.prev = { block.id };

                for (int next : remBlock.next) {
                    IR_Block &nextBlock = cfg.block(next);
                    for (int &prev : nextBlock.prev) {
                        if (prev == block.id)
                            prev = remBlock.id;
                    }
                }

                passBlock(remBlock); // Finish current block processing
                return true;
            }
        }
    }
    return false;
}

/**
 * Copies func blocks into current CFG, replaces returns with jumps into retBlock
 * and returns local entry block.
 */
IR_Block& FunctionsInliner::inlineFunc(IntermediateUnit::Function const &func, IR_Block &retBlock,
                                       IR_Node const &callingNode) {
    std::map<int, int> idsMap;
    std::vector<IR_Block*> newBlocks;
    IR_Block *newEntry = nullptr;
    for (auto const &[blockId, block] : func.cfg.getBlocks()) {
        IR_Block &clonedBlock = cfg.insertBlock(block.copy());
        idsMap.emplace(block.id, clonedBlock.id);
        newBlocks.push_back(&clonedBlock);
        if (blockId == func.cfg.entryBlockId)
            newEntry = &clonedBlock;
    }

    reenumerateRegisters(newBlocks, callingNode);

    std::map<int, IRval> retValues;
    for (IR_Block *block : newBlocks) {
        for (int &id : block->next)
            id = idsMap.at(id);
        for (int &id : block->prev)
            id = idsMap.at(id);

        auto &term = dynamic_cast<IR_ExprTerminator &>(*block->termNode.value().body);
        if (term.termType == IR_ExprTerminator::RET) {
            if (term.arg) {
                retValues.emplace(retBlock.prev.size(), std::move(*term.arg));
                term.arg = {};
            }
            block->setTerminator(IR_ExprTerminator::JUMP);
            block->next = { retBlock.id };
            retBlock.prev.push_back(block->id);
        }
    }

    if (retValues.empty()) {
        if (callingNode.res)
            throw cw39_internal_error("Functions returns nothing, but caller expects value");
    }
    else if (callingNode.res) { // If result is discarded, so skip returns consentrating
        if (retValues.size() == 1) {
            std::vector<IRval> movArgs{ std::move(retValues.extract(0).mapped()) };
            auto movExpr = std::make_unique<IR_ExprOper>(IR_ExprOper::MOV, std::move(movArgs));
            retBlock.addNode(callingNode.res, std::move(movExpr));
        }
        else {
            IR_Node &phiNode = retBlock.addNewPhiNode(*callingNode.res);
            auto &phi = dynamic_cast<IR_ExprPhi &>(*phiNode.body);
            phi.args = std::move(retValues);
        }
    }

    return *newEntry;
}

void FunctionsInliner::reenumerateRegisters(std::vector<IR_Block*> const &blocks, IR_Node const &callingNode) {
    std::map<IRval, IRval> replacementMap;

    for (IR_Block *block : blocks) {
        for (IR_Node *node : block->getAllNodes()) {
            if (!node->res)
                continue;
            auto newReg = cfg.createReg(node->res->getType());
            replacementMap.emplace(*node->res, newReg);
            *node->res = std::move(newReg);
        }
    }

    for (IR_Block *block : blocks) {
        for (IR_Node *node : block->getAllNodes()) {
            if (!node->body)
                continue;
            for (IRval *arg : node->body->getArgs()) {
                if (arg->isVReg()) {
                    auto it = replacementMap.find(*arg);
                    if (it != replacementMap.end()) {
                        *arg = it->second;
                    }
                }
                else if (arg->isFunParam()) {
                    int num = arg->castValTo<int>();
                    *arg = callingNode.body->getCall().args.at(num);
                }
            }
        }
    }
}
