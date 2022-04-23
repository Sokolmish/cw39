#include "cfg_cleaner.hpp"
#include "utils.hpp"
#include "graph_info.hpp"
#include <stack>
#include <algorithm>
#include <ranges>

namespace rng = std::ranges;

CfgCleaner::CfgCleaner(CFGraph rawCfg) : cfg(std::move(rawCfg)) {}

CFGraph const& CfgCleaner::getCfg() {
    return cfg;
}
CFGraph CfgCleaner::moveCfg() && {
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


void CfgCleaner::removeUselessNodes() {
    RegsSet_t usedRegs = getPrimaryEffectiveRegs();
    RegsSet_t extension = extendEffectiveRegsSet(usedRegs);
    while (!extension.empty()) {
        RegsSet_t extension2 = extendEffectiveRegsSet(extension);
        size_t sizeBefore = usedRegs.size();
        usedRegs.merge(extension);
        extension = std::move(extension2);
        if (sizeBefore == usedRegs.size())
            break;
    }
    usedRegs.merge(extension); // TODO: Make this function more simplier...
    removeUnusedNodes(usedRegs);
}

CfgCleaner::RegsSet_t CfgCleaner::getPrimaryEffectiveRegs() {
    std::set<IRval, IRval::Comparator> usedRegs;

    auto visitor = [this, &usedRegs](int blockId) {
        auto &curBlock = cfg.block(blockId);
        for (IR_Node const *node : curBlock.getAllNodes()) {
            if (node->body->type == IR_Expr::MEMORY) {
                auto &memExpr = node->body->getMem();
                if (memExpr.op == IR_ExprMem::STORE) {
                    usedRegs.insert(memExpr.addr);
                    usedRegs.insert(memExpr.val.value());
                }
                // TODO: volatile read
            }
            else if (node->body->type == IR_Expr::CALL) {
                auto &callExpr = node->body->getCall();
                if (callExpr.isIndirect())
                    usedRegs.insert(callExpr.getFuncPtr());
                for (auto const &arg : callExpr.args) {
                    if (arg.isVReg())
                        usedRegs.insert(arg);
                }
            }
            else if (node->body->type == IR_Expr::TERM) {
                auto &termExpr = node->body->getTerm();
                if (termExpr.arg)
                    usedRegs.insert(*termExpr.arg);
            }
        }
    };

    std::set<int> visited;
    cfg.traverseBlocks(cfg.entryBlockId, visited, visitor);
    return usedRegs;
}

CfgCleaner::RegsSet_t CfgCleaner::extendEffectiveRegsSet(RegsSet_t const &regsSet) {
    std::set<IRval, IRval::Comparator> extension;

    auto visitor = [this, &extension, &regsSet](int blockId) {
        auto &curBlock = cfg.block(blockId);
        for (IR_Node const *node : curBlock.getAllNodes()) {
            if (node->body && node->res && regsSet.contains(*node->res)) {
                for (IRval const *arg : node->body->getArgs()) {
                    extension.insert(*arg);
                }
            }
        }
    };

    std::set<int> visited;
    cfg.traverseBlocks(cfg.entryBlockId, visited, visitor);
    return extension;
}

void CfgCleaner::removeUnusedNodes(RegsSet_t const &usedRegs) {
    auto visitor = [this, &usedRegs](int blockId) {
        auto &curBlock = cfg.block(blockId);
        for (IR_Node *node : curBlock.getAllNodes()) {
            // If node has needless result (none or unused) then consider to remove it
            if (!node->res || !usedRegs.contains(*node->res)) {
                if (isInList(node->body->type, IR_Expr::CALL, IR_Expr::TERM)) {
                    node->res = {};
                }
                else if (node->body->type == IR_Expr::MEMORY) {
                    auto &memExpr = node->body->getMem();
                    if (memExpr.op == IR_ExprMem::LOAD) // TODO: volatile read
                        *node = IR_Node::nop();
                }
                else {
                    *node = IR_Node::nop();
                }
            }
        }
    };

    std::set<int> visited;
    cfg.traverseBlocks(cfg.entryBlockId, visited, visitor);
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
    cfg.traverseBlocks(cfg.entryBlockId, visited, [](int blockId) {});

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
