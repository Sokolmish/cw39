#include "vars_virtualizer.hpp"
#include "cfg_cleaner.hpp"
#include "utils.hpp"
#include <deque>
#include <set>

VarsVirtualizer::VarsVirtualizer(IntermediateUnit const &unit, CFGraph rawCfg) : IRTransformer(std::move(rawCfg)) {
    passFunction();

    CfgCleaner cleaner(unit, std::move(cfg));
    cleaner.removeNops();
    cleaner.removeUnreachableBlocks();
    cleaner.removeTransitBlocks();
    if (cleaner.isPassEffective())
        setPassChanged();
    cfg = std::move(cleaner).moveCfg();
}

void VarsVirtualizer::passFunction() {
    std::deque<int> nextBlocks;
    std::set<int> visited;

    nextBlocks.push_back(cfg.entryBlockId);
    while (!nextBlocks.empty()) {
        IR_Block &curBlock = cfg.block(nextBlocks.front());
        nextBlocks.pop_front();
        visited.insert(curBlock.id);
        analyzeBlock(curBlock);
        for (int nextId : curBlock.next)
            if (!visited.contains(nextId))
                nextBlocks.push_back(nextId);
    }

    nextBlocks.push_back(cfg.entryBlockId);
    visited.clear();
    while (!nextBlocks.empty()) {
        IR_Block &curBlock = cfg.block(nextBlocks.front());
        nextBlocks.pop_front();
        visited.insert(curBlock.id);
        optimizeBlock(curBlock);
        for (int nextId : curBlock.next)
            if (!visited.contains(nextId))
                nextBlocks.push_back(nextId);
    }
}

void VarsVirtualizer::analyzeBlock(IR_Block const &block) {
    for (IR_Node const &instr: block.body) {
        if (instr.body->type == IR_Expr::ALLOCATION && instr.res.has_value() && instr.res->isVReg()) {
            auto const &alloc = dynamic_cast<IR_ExprAlloc const &>(*instr.body);
            if (!isInList(alloc.type->type, IR_Type::DIRECT, IR_Type::POINTER))
                continue;
            if (alloc.isOnHeap)
                continue;
            toRedudeList.emplace(*instr.res, std::optional<IRval>());
        }
        else {
            auto oper = dynamic_cast<IR_ExprMem const *>(instr.body.get());
            if (oper) {
                if (oper->op == IR_ExprMem::LOAD) {
                    continue;
                }
                else if (oper->op == IR_ExprMem::STORE) {
                    auto it = toRedudeList.find(*oper->val);
                    if (it != toRedudeList.end())
                        toRedudeList.erase(it);
                    continue;
                }
            }

            for (auto arg: instr.body->getArgs()) {
                auto it = toRedudeList.find(*arg);
                if (it != toRedudeList.end())
                    toRedudeList.erase(it);
            }
        }
    }
}

void VarsVirtualizer::optimizeBlock(IR_Block &block) {
    for (IR_Node &instr : block.body) {
        if (!instr.body)
            continue;

        // Remove alloca instruction
        if (instr.res) {
            auto it = toRedudeList.find(*instr.res);
            if (it != toRedudeList.end()) {
                auto const &alloc = dynamic_cast<IR_ExprAlloc const &>(*instr.body);
                it->second = cfg.createReg(alloc.type);
                instr = IR_Node::nop();
                setPassChanged();
                continue;
            }
        }

        // Replace memory instructions
        if (instr.body->type == IR_Expr::MEMORY) {
            auto oper = dynamic_cast<IR_ExprMem const *>(instr.body.get());
            if (oper) {
                if (oper->op == IR_ExprMem::STORE) {
                    auto it = toRedudeList.find(oper->addr);
                    if (it != toRedudeList.end()) {
                        instr.res = it->second;
                        instr.body = std::make_unique<IR_ExprOper>(
                                IR_ExprOper::MOV, std::vector<IRval>{ *oper->val });
                        setPassChanged();
                    }
                }
                else if (oper->op == IR_ExprMem::LOAD) {
                    auto it = toRedudeList.find(oper->addr);
                    if (it != toRedudeList.end()) {
                        instr.body = std::make_unique<IR_ExprOper>(
                                IR_ExprOper::MOV, std::vector<IRval>{ *it->second });
                        setPassChanged();
                    }
                }
            }
        }

        // Replace usages
        if (instr.body) {
            for (auto arg: instr.body->getArgs()) {
                auto it = toRedudeList.find(*arg);
                if (it != toRedudeList.end()) {
                    *arg = *it->second;
                    setPassChanged();
                }
            }
        }
    }

    // TODO: use getAllNodes
    if (block.termNode.has_value()) {
        auto &terminator = dynamic_cast<IR_ExprTerminator &>(*block.termNode->body);
        if (terminator.arg.has_value()) {
            auto it = toRedudeList.find(*terminator.arg);
            if (it != toRedudeList.end()) {
                terminator.arg = *it->second;
                setPassChanged();
            }
        }
    }
}
