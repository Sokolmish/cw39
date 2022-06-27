#include "cfg_cleaner.hpp"
#include "utils.hpp"
#include <stack>
#include <algorithm>
#include <ranges>

namespace rng = std::ranges;

CfgCleaner::CfgCleaner(IntermediateUnit const &unit, CFGraph rawCfg)
        : IRTransformer(std::move(rawCfg)), iunit(&unit) {}

void CfgCleaner::removeNops() {
    for (auto const &[bId, block] : cfg.getBlocks()) {
        std::vector<std::unique_ptr<IR_Expr>> newPhis;
        newPhis.reserve(cfg.block(bId).phis.size());
        for (auto &node : cfg.block(bId).phis) {
            if (!node->isNop())
                newPhis.push_back(std::move(node));
            else
                setPassChanged();
        }
        cfg.block(bId).phis = std::move(newPhis);

        std::vector<std::unique_ptr<IR_Expr>> newBody;
        for (auto &node : cfg.block(bId).body) {
            if (!node->isNop())
                newBody.push_back(std::move(node));
            else
                setPassChanged();
        }
        cfg.block(bId).body = std::move(newBody);
    }
}


void CfgCleaner::removeUselessNodes() {
    std::set<IRval> usedRegs = getPrimaryEffectiveRegs();
    std::set<IRval> extension = extendEffectiveRegsSet(usedRegs);
    while (!extension.empty()) {
        std::set<IRval> extension2 = extendEffectiveRegsSet(extension);
        size_t sizeBefore = usedRegs.size();
        usedRegs.merge(extension);
        extension = std::move(extension2);
        if (sizeBefore == usedRegs.size())
            break;
    }
    usedRegs.merge(extension); // TODO: Make this function more simple...
    removeUnusedNodes(usedRegs);
}

std::set<IRval> CfgCleaner::getPrimaryEffectiveRegs() {
    std::set<IRval> usedRegs;

    auto visitor = [this, &usedRegs](int blockId) {
        auto &curBlock = cfg.block(blockId);
        for (IR_Expr const *node : curBlock.getAllNodes()) {
            if (auto memExpr = node->toMem()) {
                if (memExpr->op == IR_ExprMem::STORE) {
                    usedRegs.insert(memExpr->addr);
                    usedRegs.insert(memExpr->val.value());
                }
                // TODO: volatile read
            }
            else if (auto callExpr = node->toCall()) {
                if (callExpr->isIndirect()) {
                    usedRegs.insert(callExpr->getFuncPtr());
                }
                else {
                    auto const &func = iunit->getFunction(callExpr->getFuncId());
                    if (func.isPure())
                        continue;
                }
                for (auto const &arg : callExpr->args) {
                    if (arg.isVReg())
                        usedRegs.insert(arg);
                }
            }
            else if (auto termExpr = node->toTerm()) {
                if (termExpr->arg)
                    usedRegs.insert(*termExpr->arg);
            }
        }
    };

    std::set<int> visited;
    cfg.traverseBlocks(cfg.entryBlockId, visited, visitor);
    return usedRegs;
}

std::set<IRval> CfgCleaner::extendEffectiveRegsSet(std::set<IRval> const &regsSet) {
    std::set<IRval> extension;

    auto visitor = [this, &extension, &regsSet](int blockId) {
        auto &curBlock = cfg.block(blockId);
        for (IR_Expr *node : curBlock.getAllNodes()) {
            if (node->res && regsSet.contains(*node->res)) {
                for (IRval const *arg : node->getArgs()) {
                    extension.insert(*arg);
                }
            }
        }
    };

    std::set<int> visited;
    cfg.traverseBlocks(cfg.entryBlockId, visited, visitor);
    return extension;
}

void CfgCleaner::removeUnusedNodes(std::set<IRval> const &usedRegs) {
    auto visitor = [this, &usedRegs](int blockId) {
        auto &curBlock = cfg.block(blockId);
        for (IR_Expr *node : curBlock.getAllNodes()) {
            // If node has needless result (none or unused) then consider to remove it
            if (!node->res || !usedRegs.contains(*node->res)) {
                if (node->toCall()) {
                    node->res = {};
                    setPassChanged();
                }
                else if (auto memExpr = node->toMem()) {
                    if (memExpr->op == IR_ExprMem::LOAD) { // TODO: volatile read
                        *node = IR_ExprNOP();
                        setPassChanged();
                    }
                }
                else if (!node->toTerm()) {
                    *node = IR_ExprNOP();
                    setPassChanged();
                }
            }
        }
    };

    std::set<int> visited;
    cfg.traverseBlocks(cfg.entryBlockId, visited, visitor);
}


void CfgCleaner::removeTransitBlocks() {
    removeUselessBranches();

    std::vector<int> toRemoveList;
    for (auto &[bId, block] : cfg.getBlocksData()) {
        if (block.body.empty() && block.phis.empty()) {
            if (block.getTerminator()->termType == IR_ExprTerminator::JUMP) {
                if (block.next.size() != 1)
                    throw cw39_internal_error("JUMP node with more than 1 successors");

                if (block.prev.size() != 1) // Concentrating block
                    continue;

                IR_Block &prevBlock = cfg.block(block.prev[0]);
                IR_Block &nextBlock = cfg.block(block.next[0]);

                // TODO: phis folding
                if (!nextBlock.phis.empty()) {
                    if (rng::find(nextBlock.prev, block.prev[0]) != nextBlock.next.end())
                        continue;
                }

                // Relink previous blocks
                for (int &refId : prevBlock.next) {
                    if (refId == bId) {
                        refId = block.next[0];
                        break;
                    }
                }

                // Relink next
                for (int &refId : nextBlock.prev) {
                    if (refId == bId) {
                        refId = block.prev[0];
                        break;
                    }
                }

                // Remove block
                toRemoveList.push_back(bId); // TODO: iterators?
            }
        }
    }

    for (int id : toRemoveList) {
        cfg.getBlocksData().erase(id);
        setPassChanged();
    }

    // In case if some dual branches were created
    removeUselessBranches();
}

void CfgCleaner::removeUselessBranches() {
    for (auto &[bId, block] : cfg.getBlocksData()) {
        if (block.getTerminator()->termType == IR_ExprTerminator::BRANCH) {
            if (block.next.at(0) == block.next.at(1)) {
                block.next = { block.next.at(0) };
                block.setTerminator(IR_ExprTerminator::JUMP);
                setPassChanged();
            }
        }
    }
}


// TODO: refactor this
void CfgCleaner::removeUnreachableBlocks() {
    std::set<int> visited;
    cfg.traverseBlocks(cfg.entryBlockId, visited, [](int blockId) {});

    std::set<int> toRemoveList;
    for (auto &[bId, block] : cfg.getBlocksData())
        if (!visited.contains(bId))
            toRemoveList.insert(bId);

    for (int id : toRemoveList) {
        cfg.getBlocksData().erase(id);
        setPassChanged();
    }

    for (auto &[bId, block] : cfg.getBlocksData()) {
        std::vector<int> newPrev;
        bool changed = false;
        for (int prev : block.prev) {
            if (!toRemoveList.contains(prev))
                newPrev.push_back(prev);
            else
                changed = true;
        }
        if (changed) {
            block.prev = std::move(newPrev);
            setPassChanged();
        }
    }

#if 0
    toRemoveList.clear();

    for (auto &[bId, block] : cfg.getBlocksData()) {
        if (block.getTerminator()->termType == IR_ExprTerminator::BRANCH) {
            if (block.getTerminator()->arg->getValueClass() == IRval::VAL) {
                int toRemoveId;
                if (block.getTerminator()->arg->castValTo<uint64_t>() != 0) {
                    toRemoveId = block.next[1];
                    block.next = { block.next[0] };
                }
                else {
                    toRemoveId = block.next[0];
                    block.next = { block.next[1] };
                }

                // TODO: because we can remove some predcessors, some PHIs can become broken
                // cfg.block(toRemoveId).removePredecessor(block.id);

                std::set<int> unreached = getDominatedByGiven(toRemoveId);
                toRemoveList.insert(unreached.begin(), unreached.end());
                block.setTerminator(IR_ExprTerminator::JUMP);
                setPassChanged();
            }
        }
    }

    for (int id : toRemoveList) {
        cfg.getBlocksData().erase(id);
        setPassChanged();
    }
    for (auto &[bId, block] : cfg.getBlocksData()) { // TODO: duplicate
        std::vector<int> newPrev;
        bool changed = false;
        for (int prev : block.prev) {
            if (!toRemoveList.contains(prev))
                newPrev.push_back(prev);
            else
                changed = true;
        }
        if (changed) {
            block.prev = std::move(newPrev);
            setPassChanged();
        }
    }
#endif
}

std::set<int> CfgCleaner::getDominatedByGiven(int startId) {
    if (cfg.block(startId).prev.size() != 1)
        return {};
    GraphInfo gInfo(cfg);
    std::set<int> res { startId };
    std::stack<int> stack;
    stack.push(startId);
    while (!stack.empty()) {
        IR_Block const &curBlock = cfg.block(stack.top());
        stack.pop();

        for (int next : curBlock.next) {
            if (!res.contains(next) && gInfo.isDom(startId, next)) {
                stack.push(next);
                res.insert(next);
            }
        }
    }
    return res;
}



/** Returns true if node is write, volatile read, non-pure call or return */
bool CfgCleaner::isNodeGeneralEffective(IR_Expr const &node) {
    if (auto memExpr = node.toMem()) {
        if (memExpr->op == IR_ExprMem::STORE)
            return true;
        // TODO: volatile read
    }
    else if (auto callExpr = node.toCall()) {
        if (callExpr->isIndirect())
            return true;
        auto const &func = iunit->getFunction(callExpr->getFuncId());
        if (!func.isPure())
            return true;
    }
    else if (auto termExpr = node.toTerm()) {
        if (termExpr->termType == IR_ExprTerminator::RET)
            return true;
    }
    return false;
}

bool CfgCleaner::isLoopEffective(LoopNode const &loop) {
    std::set<IRval> assignedRegs;

    bool alreadyExited = false; // Can have only 1 exit from loop
    for (int blockId : loop.blocks) {
        IR_Block &block = cfg.block(blockId);
        for (IR_Expr const *node : block.getAllNodes()) {
            if (isNodeGeneralEffective(*node))
                return true;
            if (auto termExpr = node->toTerm()) {
                if (termExpr->termType == IR_ExprTerminator::BRANCH) {
                    for (int next : block.next) {
                        if (!loop.blocks.contains(next)) {
                            if (alreadyExited)
                                return true;
                            else
                                alreadyExited = true;
                        }
                    }
                }
            }
            // Here node is primary ineffective
            if (node->res) {
                assignedRegs.insert(*node->res);
            }
        }
    }

    std::set<int> visited;
    bool loopSubEffective = false;
    auto visitor = [this, &loop, &assignedRegs, &loopSubEffective](int blockId) {
        if (loop.blocks.contains(blockId))
            return;
        IR_Block &block = cfg.block(blockId);
        for (IR_Expr *node : block.getAllNodes()) {
            for (IRval const *arg : node->getArgs()) {
                if (assignedRegs.contains(*arg)) {
                    loopSubEffective = true;
                    return; // TODO: early stop traversing
                }
            }
        }
    };
    cfg.traverseBlocks(cfg.entryBlockId, visited, visitor);
    return loopSubEffective;
}

/**
 * Assume that loop has only 1 exit.
 * First one is block inside loop, second one -- outside.
 */
std::pair<int, int> CfgCleaner::getLoopExit(LoopNode const &loop) {
    for (int blockId : loop.blocks) {
        IR_Block const &block = cfg.block(blockId);
        auto const &term = block.getTerminator();
        if (term->termType == IR_ExprTerminator::BRANCH) {
            for (int next : block.next) {
                if (!loop.blocks.contains(next))
                    return { blockId, next };
            }
        }
    }
    throw cw39_internal_error("Wrong loop structure");
}

void CfgCleaner::removeUselessLoops() {
    LoopsDetector loops(cfg);

    for (auto &[loopId, loop] : loops.getLoops()) {
        if (!isLoopEffective(loop)) {
            // If loop is ineffective, it can have only 1 exit
            auto loopExit = getLoopExit(loop);

            IR_Block &head = cfg.block(loop.head.id);

            IR_Block &cBlock = cfg.createBlock();
            cBlock.phis = std::move(head.phis);
            cBlock.setTerminator(IR_ExprTerminator::JUMP);
            cBlock.next.push_back(loopExit.second);
            cBlock.prev = std::move(head.prev);
            for (int prev : cBlock.prev) {
                IR_Block &prevBlock = cfg.block(prev);
                for (int &next : prevBlock.next) {
                    if (next == head.id)
                        next = cBlock.id;
                }
            }

            IR_Block &postExitBlock = cfg.block(loopExit.second);
            for (int &prev : postExitBlock.prev) {
                if (prev == head.id)
                    prev = cBlock.id;
            }

            setPassChanged();
        }
    }
}
