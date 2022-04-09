#include "cfg_cleaner.hpp"
#include "utils.hpp"
#include "graph_info.hpp"
#include <stack>
#include <algorithm>
#include <ranges>

namespace rng = std::ranges;

CfgCleaner::CfgCleaner(ControlFlowGraph rawCfg) : cfg(std::move(rawCfg)) {}

ControlFlowGraph const& CfgCleaner::getCfg() {
    return cfg;
}
ControlFlowGraph CfgCleaner::moveCfg() && {
    return std::move(cfg);
}


void CfgCleaner::removeNops() {
    for (auto const &[bId, block] : cfg.getBlocks()) {
        std::vector<IR_Node> newPhis;
        for (IR_Node &node : cfg.block(bId).phis)
            if (node.body)
                newPhis.push_back(std::move(node));
        cfg.block(bId).phis = std::move(newPhis);

        std::vector<IR_Node> newBody;
        for (IR_Node &node : cfg.block(bId).body)
            if (node.body)
                newBody.push_back(std::move(node));
        cfg.block(bId).body = std::move(newBody);
    }
}

void CfgCleaner::fixVersions() {
    std::set<int> visited;
    std::map<IRval, std::pair<bool, int>, IRval::ComparatorIgnoreVers> versionedRegs;

    visited.clear();
    for (auto const &[fId, func]: cfg.getFuncs()) {
        cfg.traverseBlocks(func.getEntryBlockId(), visited, [this, &versionedRegs](int blockId) {
            auto &curBlock = cfg.block(blockId);
            for (IR_Node *node: curBlock.getAllNodes()) {
                if (node->res) {
                    int curVers = *node->res->getVersion();
                    auto it = versionedRegs.lower_bound(*node->res);
                    if (it != versionedRegs.end() && it->first.equalIgnoreVers(*node->res)) {
                        if (it->second.second != curVers)
                            it->second = std::make_pair(true, 0);
                    }
                    else {
                        versionedRegs.emplace_hint(it, *node->res, std::make_pair(false, curVers));
                    }
                }
            }
        });
    }

    visited.clear();
    for (auto const &[fId, func]: cfg.getFuncs()) {
        cfg.traverseBlocks(func.getEntryBlockId(), visited, [this, &versionedRegs](int blockId) {
            auto &curBlock = cfg.block(blockId);

            for (auto *node: curBlock.getAllNodes()) {
                if (node->res) {
                    auto it = versionedRegs.find(*node->res);
                    if (it != versionedRegs.end() && !it->second.first)
                        node->res->dropVersion();
                }
                for (IRval *arg : node->body->getArgs()) {
                    if (arg->isUndefVersion()) {
                        *arg = IRval::createUndef(arg->getType());
                    }
                    else {
                        auto it = versionedRegs.find(*arg);
                        if (it != versionedRegs.end() && !it->second.first)
                            arg->dropVersion();
                    }
                }
            }
        });
    }
}

// TODO: PHI loop dependencies
void CfgCleaner::removeUselessNodes() {
    std::set<int> visited;
    std::set<IRval, IRval::Comparator> usedRegs;
    bool changed = true;

    while (changed) {
        changed = false;
        usedRegs.clear();

        visited.clear();
        for (auto const &[fId, func]: cfg.getFuncs()) {
            cfg.traverseBlocks(func.getEntryBlockId(), visited, [this, &usedRegs](int blockId) {
                auto &curBlock = cfg.block(blockId);
                auto refs = curBlock.getReferences();
                usedRegs.insert(refs.begin(), refs.end());
            });
        }

        visited.clear();
        for (auto const &[fId, func]: cfg.getFuncs()) {
            cfg.traverseBlocks(func.getEntryBlockId(), visited, [this, &usedRegs, &changed](int blockId) {
                auto &curBlock = cfg.block(blockId);
                for (auto *node: curBlock.getAllNodes()) {
                    if (node->res && node->res->isVReg() && !usedRegs.contains(*node->res)) {
                        changed = true;
                        node->res = {};

                        auto const &body = node->body;
                        if (body->type == IR_Expr::CALL)
                            continue;
                        else if (body->type == IR_Expr::MEMORY && body->getMem().op == IR_ExprMem::STORE)
                            continue;
                        node->body = nullptr;
                    }
                }
            });
        }
    }
}

// TODO: transit blocks with brach and non-empty transit blocks
// TODO: condition with equal destinations
void CfgCleaner::removeTransitBlocks() {
    std::vector<int> toRemoveList;

    for (auto &[bId, block] : cfg.getBlocksData()) {
        if (block.body.empty() && block.phis.empty()) {
            if (block.getTerminator()->termType == IR_ExprTerminator::JUMP) {
                if (block.next.size() != 1)
                    internalError("JUMP node with more than 1 successors");

                // TODO: complex relink with changing `prev` size
                if (block.prev.size() != 1) // Concentrating block
                    continue;

                IR_Block &prevBlock = cfg.block(block.prev[0]);
                IR_Block &nextBlock = cfg.block(block.next[0]);

                // TODO: select op and phis folding
                if (!nextBlock.phis.empty()) {
                    if (rng::find(nextBlock.prev, block.prev[0]) != nextBlock.next.end())
                        continue;
                }

                // TODO: check if prev block already linked with next

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

    for (int id : toRemoveList)
        cfg.getBlocksData().erase(id);
}

// tODO: refactor this
void CfgCleaner::removeUnreachableBlocks() {
    std::set<int> visited;
    for (auto const &[fId, func]: cfg.getFuncs())
        cfg.traverseBlocks(func.getEntryBlockId(), visited, [](int blockId) {});

    std::set<int> toRemoveList;
    for (auto &[bId, block] : cfg.getBlocksData())
        if (!visited.contains(bId))
            toRemoveList.insert(bId);

    for (int id : toRemoveList)
        cfg.getBlocksData().erase(id);
    for (auto &[bId, block] : cfg.getBlocksData()) {
        std::vector<int> newPrev;
        bool changed = false;
        for (int prev : block.prev) {
            if (!toRemoveList.contains(prev))
                newPrev.push_back(prev);
            else
                changed = true;
        }
        if (changed)
            block.prev = std::move(newPrev);
    }

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

                std::set<int> unreached = getDominatedByGiven(toRemoveId);
                toRemoveList.insert(unreached.begin(), unreached.end());
                block.setTerminator(IR_ExprTerminator::JUMP);
            }
        }
    }

    for (int id : toRemoveList)
        cfg.getBlocksData().erase(id);
    for (auto &[bId, block] : cfg.getBlocksData()) { // TODO: duplicate
        std::vector<int> newPrev;
        bool changed = false;
        for (int prev : block.prev) {
            if (!toRemoveList.contains(prev))
                newPrev.push_back(prev);
            else
                changed = true;
        }
        if (changed)
            block.prev = std::move(newPrev);
    }
}

std::set<int> CfgCleaner::getDominatedByGiven(int startId) {
    if (cfg.block(startId).prev.size() != 1)
        return {};
    GraphInfo gInfo(cfg); // TODO: dominators for subgraph (at least function)
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
