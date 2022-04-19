#include "loops.hpp"
#include <stack>
#include <fmt/core.h>

Looper::LoopNode::LoopNode(IR_Block const &head) : head(head) {}


Looper::Looper(CFGraph const &in) : cfg(std::move(in)), gInfo(cfg) {
    // Find loops heads using back arcs
    for (auto const &[from, to] : gInfo.getArcs(GraphInfo::BACK)) {
        if (!gInfo.isDom(to, from)) { // Improper loop (because of goto)
            return; // TODO: what to do with this?
        }

        auto it = loops.lower_bound(to);
        if (it == loops.end() || it->first != to)
            it = loops.emplace_hint(it, to, cfg.block(to));
        it->second.ends.insert(from);
    }

    // Find loops bodies
    for (auto &[loopId, loop] : loops) {
        for (int tailId : loop.ends) {
            traverseLoopBranch(loop, tailId);
        }
    }

    std::set<std::pair<int, int>> loopsRels;

    // Set associated blocks
    for (auto &[loopId, loop] : loops) {
        for (int blockId : loop.blocks) {
            lblocks[blockId].loops.insert(loopId);

            for (int othLoop : lblocks[blockId].loops) {
                if (othLoop != loopId) {
                    loopsRels.insert({ std::min(othLoop, loopId), std::max(othLoop, loopId) });
                }
            }
        }
    }

    // Set loops relations (who is nested in whom)
    for (auto const &[l1, l2] : loopsRels) {
//        fmt::print("{} - {}\n", l1, l2);
        if (!testLoopsRelation(loops.at(l1), loops.at(l2))) {
            fmt::print("Improper loop"); // TODO
        }
    }
}

void Looper::traverseLoopBranch(LoopNode &loop, int tailId) {
    std::set<int> visited;
    std::stack<int> stack;

    visited.insert(loop.head.id);
    visited.insert(tailId);
    stack.push(tailId);

    while (!stack.empty()) {
        IR_Block const &block = cfg.block(stack.top());
        stack.pop();
        for (int next : block.prev) {
            if (!visited.contains(next)) {
                visited.insert(next);
                stack.push(next);
            }
        }
    }

    loop.blocks.merge(visited);
}

void Looper::draw() const {
    for (auto &[loopId, loop] : loops) {
        fmt::print("loop {}:", loop.head.id);
        for (int blockId : loop.blocks)
            fmt::print(" {}", blockId);
        fmt::print("\n");
    }
}

bool Looper::testLoopsRelation(LoopNode &sLoop, LoopNode &lLoop) {
    if (sLoop.blocks.size() > lLoop.blocks.size()) {
        return testLoopsRelation(lLoop, sLoop);
    }

    bool test = std::includes(lLoop.blocks.cbegin(), lLoop.blocks.cend(),
                              sLoop.blocks.cbegin(), sLoop.blocks.cend());
    if (test) {
        sLoop.parent = &lLoop;
        return true;
    }
    else {
        return false;
    }
}
