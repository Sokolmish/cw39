#include "loop_inv_mover.hpp"
#include "cfg_cleaner.hpp"
#include <stack>

LoopInvMover::LoopInvMover(CFGraph rawCfg) : IRTransformer(std::move(rawCfg)), loops(cfg) {
    std::vector<LoopNode const *> topLoops;
    for (auto const &[lId, loop] : loops.getLoops()) {
        if (loop.parent == nullptr)
            topLoops.push_back(&loop);
    }
    for (LoopNode const *loop : topLoops) {
        passNestedLoops(*loop);
    }

    CfgCleaner cleaner(std::move(cfg));
    cleaner.removeNops();
    cleaner.removeTransitBlocks();
    cfg = std::move(cleaner).moveCfg();
}

void LoopInvMover::passNestedLoops(const LoopNode &topLoop) {
    passLoop(topLoop);
    for (LoopNode const *nest : topLoop.children) {
        passNestedLoops(*nest);
    }
}

static bool isNodeInvariant(IR_Node const &node, std::set<IRval> const &varsInLoop) {
    auto args = node.body->getArgs();
    for (IRval const *arg : args) {
        if (varsInLoop.contains(*arg))
            return false;
    }
    return true;
}

void LoopInvMover::passLoop(LoopNode const &loop) {
    std::set<IRval> varsInLoop;
    auto visitor1 = [this, &varsInLoop](int blockId) {
        IR_Block &block = cfg.block(blockId);
        for (auto const &node : block.getAllNodes()) {
            if (node->res) {
                varsInLoop.insert(*node->res);
            }
        }
    };
    traverseLoop(loop, visitor1);

    std::vector<IR_Node> invNodes;
    auto visitor2 = [this, &varsInLoop, &invNodes](int blockId) {
        IR_Block &block = cfg.block(blockId);
        for (auto &node : block.body) {
            if (node.body) {
                if (isInList(node.body->type, IR_Expr::CALL, IR_Expr::ACCESS, IR_Expr::ALLOCATION))
                    continue;
                if (isNodeInvariant(node, varsInLoop)) {
                    if (node.res.has_value())
                        varsInLoop.erase(*node.res);
                    invNodes.emplace_back(std::exchange(node, IR_Node::nop()));
                }
            }
        }
    };

    size_t lastSize;
    do {
        lastSize = varsInLoop.size();
        traverseLoop(loop, visitor2);
    } while (varsInLoop.size() != lastSize);

    if (!invNodes.empty()) {
        moveInvariantNodes(loop, std::move(invNodes));
    }
}

void LoopInvMover::traverseLoop(const LoopNode &loop, const std::function<void(int)> &visit) {
    std::set<int> visited;
    std::stack<int> stack;
    stack.push(loop.head.id);
    visited.insert(loop.head.id);

    while (!stack.empty()) {
        int cur = stack.top();
        stack.pop();
        visited.insert(cur);
        visit(cur);

        if (loop.ends.contains(cur))
            continue;

        IR_Block &block = cfg.block(cur);
        for (int next : block.next) {
            if (!visited.contains(next))
                stack.push(next);
        }
    }
}

void LoopInvMover::moveInvariantNodes(const LoopNode &loop, std::vector<IR_Node> &&invNodes) {
    IR_Block &prehead = cfg.createBlock();
    IR_Block &head = cfg.block(loop.head.id);

    prehead.body = std::move(invNodes);

    // Relink external blocks from head to prehead (both directions)
    for (int prev : head.prev) {
        if (loop.ends.contains(prev))
            continue;
        IR_Block &prevBlock = cfg.block(prev);
        for (int &next : prevBlock.next) {
            if (next == head.id) {
                next = prehead.id;
                prehead.prev.push_back(prev); // predcessor
                break; // No double arcs
            }
        }
    }

    // Link prehead with head (no predcessor at this time)
    prehead.setTerminator(IR_ExprTerminator::JUMP);
    prehead.next.push_back(loop.head.id);

    if (head.prev.size() - 1 == loop.ends.size()) {
        for (int &prev : head.prev) { // Change single external arg with preheader
            if (!loop.ends.contains(prev)) {
                prev = prehead.id;
                break;
            }
        }
    }
    else {
        throw cw39_not_implemented("Many external arcs in loop head"); // TODO
//        for (IR_Node const &phi : head.phis) {}
    }
}
