#include "vars_virtualizer.hpp"
#include <deque>
#include <set>

VarsVirtualizer::VarsVirtualizer(ControlFlowGraph rawCfg)
        : cfg(std::make_shared<ControlFlowGraph>(std::move(rawCfg))) {
    for (auto const &[id, func] : cfg->funcs) {
        toRedudeList.clear();
        passFunction(func);
    }
}

std::shared_ptr<ControlFlowGraph> VarsVirtualizer::getCfg() {
    return cfg;
}


void VarsVirtualizer::passFunction(const ControlFlowGraph::Function &func) {
    std::deque<int> nextBlocks;
    std::set<int> visited;

    nextBlocks.push_back(func.getEntryBlockId());
    while (!nextBlocks.empty()) {
        IR_Block &curBlock = cfg->block(nextBlocks.front());
        nextBlocks.pop_front();
        visited.insert(curBlock.id);
        analyzeBlock(curBlock);
        for (int nextId : curBlock.next)
            if (!visited.contains(nextId))
                nextBlocks.push_back(nextId);
    }

    nextBlocks.push_back(func.getEntryBlockId());
    visited.clear();
    while (!nextBlocks.empty()) {
        IR_Block &curBlock = cfg->block(nextBlocks.front());
        nextBlocks.pop_front();
        visited.insert(curBlock.id);
        optimizeBlock(curBlock);
        for (int nextId : curBlock.next)
            if (!visited.contains(nextId))
                nextBlocks.push_back(nextId);
    }
}

void VarsVirtualizer::analyzeBlock(IR_Block const &block) {
    for (IR_Node const &instr : block.body) {
        if (instr.body->type == IR_Expr::ALLOCATION && instr.res.has_value()) {
            auto const &alloc = dynamic_cast<IR_ExprAlloc const &>(*instr.body);
            if (!alloc.isOnHeap)
                toRedudeList.emplace(*instr.res, std::optional<IRval>());
        }
        else {
            auto oper = dynamic_cast<IR_ExprOper const *>(instr.body.get());
            if (oper) {
                if (oper->op == IR_LOAD) {
                    continue;
                }
                else if (oper->op == IR_STORE) {
                    auto it = toRedudeList.find(oper->args[1]);
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
                it->second = cfg->createReg(alloc.type);
                instr.res = {};
                instr.body = nullptr;
                continue;
            }
        }

        // Replace store instructions
        if (instr.body->type == IR_Expr::OPERATION) {
            auto oper = dynamic_cast<IR_ExprOper const *>(instr.body.get());
            if (oper) {
                if (oper->op == IR_STORE) {
                    auto it = toRedudeList.find(oper->args[0]);
                    if (it != toRedudeList.end()) {
                        instr.res = it->second;
                        instr.body = std::make_unique<IR_ExprOper>(
                                IR_MOV, std::vector<IRval>{ oper->args[1] });
                    }
                }
                else if (oper->op == IR_LOAD) {
                    auto it = toRedudeList.find(oper->args[0]);
                    if (it != toRedudeList.end()) {
                        instr.body = std::make_unique<IR_ExprOper>(
                                IR_MOV, std::vector<IRval>{ *it->second });
                    }
                }
            }
        }

        // Replace usages
        if (instr.body) {
            for (auto arg: instr.body->getArgs()) {
                auto it = toRedudeList.find(*arg);
                if (it != toRedudeList.end())
                    *arg = *it->second;
            }
        }
    }

    if (block.terminator.arg.has_value()) {
        auto it = toRedudeList.find(*block.terminator.arg);
        if (it != toRedudeList.end())
            block.terminator.arg = *it->second;
    }
}
