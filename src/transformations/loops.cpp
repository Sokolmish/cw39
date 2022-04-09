#include "loops.hpp"
#include <stack>
#include <fmt/core.h>

Looper::Looper(std::shared_ptr<ControlFlowGraph> in) : cfg(std::move(in)), gInfo(*cfg) {
    for (auto &[bId, block] : cfg->getBlocksData()) {
        lblocks.emplace_hint(lblocks.end(), bId, BlockInfo(block));
    }

    for (auto const &[from, to] : gInfo.getArcs(GraphInfo::TREE)) {
        spanningTree[from].push_back(to);
    }
    for (auto const &[from, to] : gInfo.getArcs(GraphInfo::BACK)) {
        auto it = loops.lower_bound(to);
        if (it == loops.end() || it->first != to)
            it = loops.emplace_hint(it, to, cfg->block(to));
        it->second.ends.insert(from);
    }

    for (auto const &[fId, func] : cfg->getFuncs()) {
        dfs1(func.getEntryBlockId(), nullptr);
    }
    for (auto &[id, loop] : loops) {
        dfs2(loop.block.id, {}, &loop, loop.ends);
    }
}

void Looper::dfs1(int nodeId, LoopNode *curLoop) {
    auto loopIt = loops.find(nodeId);
    if (loopIt != loops.end()) {
        loopIt->second.parent = curLoop;
        curLoop = &loopIt->second;
    }

    auto nextIt = spanningTree.find(nodeId);
    if (nextIt != spanningTree.end()) {
        for (int nextId : nextIt->second) {
            dfs1(nextId, curLoop);
        }
    }
}

void Looper::dfs2(int nodeId, std::vector<int> path, LoopNode *curLoop, std::set<int> ends) {
    auto endIt = ends.find(nodeId);
    if (endIt != ends.end()) {
        for (auto it = path.rbegin(); it != path.rend(); it++) {
            BlockInfo &block = lblocks.at(*it);
            if (block.loop != nullptr && isLoopNested(block.loop, curLoop))
                continue;
            block.loop = curLoop;
        }

        ends.erase(endIt);
        if (ends.empty())
            return;
    }

    auto nextIt = spanningTree.find(nodeId);
    if (nextIt != spanningTree.end()) {
        path.push_back(nodeId);
        for (int nextId : nextIt->second) {
            dfs2(nextId, path, curLoop, ends);
        }
    }
}

bool Looper::isLoopNested(const LoopNode *parent, const LoopNode *child) const {
    while (child != nullptr) {
        if (child->block.id == parent->block.id)
            return true;
        child = child->parent;
    }
    return false;
}

void Looper::draw() const {
    fmt::print("graph {{ ");
    for (auto const &[id, loop] : loops) {
//        fmt::print("{}; ", id);
        if (loop.parent)
            fmt::print("{} -- {}; ", loop.parent->block.id, id);
    }
    fmt::print("}}\n");
}


Looper::BlockInfo::BlockInfo(IR_Block &bb) : block(bb) {}

Looper::LoopNode::LoopNode(IR_Block &bb) : block(bb) {}
