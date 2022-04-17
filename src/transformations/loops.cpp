#include "loops.hpp"
#include <stack>
#include <fmt/core.h>

Looper::BlockInfo::BlockInfo(IR_Block const &bb) : block(bb) {}

Looper::LoopNode::LoopNode(IR_Block const &bb) : head(bb) {}


Looper::Looper(CFGraph const &in) : cfg(std::move(in)), gInfo(cfg) {
    for (auto const &[from, to] : gInfo.getArcs(GraphInfo::BACK)) {
//        auto it = loops.lower_bound(to);
//        if (it == loops.end() || it->first != to)
//            it = loops.emplace_hint(it, to, cfg.block(to));
//        it->second.ends.insert(from);
//        if (gInfo.isDom())
    }


    // Find loops heads using back arcs
    for (auto const &[from, to] : gInfo.getArcs(GraphInfo::BACK)) {
        auto it = loops.lower_bound(to);
        if (it == loops.end() || it->first != to)
            it = loops.emplace_hint(it, to, cfg.block(to));
        it->second.ends.insert(from);
    }

    findLoopsBodies();

//    if (func.fspec & IntermediateUnit::Function::GOTOED) {
//        continue; // TODO: strongly linked components
//    }
//    else {
//        findLoopsInNormalFunc(func);
//    }

}

void Looper::draw() const {
//    fmt::print("graph {{ ");
//    for (auto const &[id, loop] : loops) {
////        fmt::print("{}; ", id);
//        if (loop.parent)
//            fmt::print("{} -- {}; ", loop.parent->head.id, id);
//    }
//    fmt::print("}}\n");
    for (auto const &[lId, loop] : loops) {
        fmt::print("loop {}:", lId);
        for (auto const &block : loop.blocks) {
            fmt::print(" {}", block);
        }
        fmt::print("\n");
    }
}


void Looper::findLoopsBodies() {
    for (auto const &[from, to] : gInfo.getArcs(GraphInfo::TREE))
        straightPaths[from].push_back(to);
    for (auto const &[from, to] : gInfo.getArcs(GraphInfo::CROSS))
        straightPaths[from].push_back(to);
    for (auto const &[from, to] : gInfo.getArcs(GraphInfo::FWD))
        straightPaths[from].push_back(to);

    for (auto &[id, loop] : loops) {
        dfs3(loop, loop.head.id, {});
    }
}

void Looper::dfs3(LoopNode &loop, int nodeId, std::vector<int> curPath) {
    if (loop.ends.contains(nodeId)) {
        loop.blocks.insert(nodeId);
        for (auto pathElem : curPath)
            loop.blocks.insert(pathElem);
        return;
    }
    curPath.push_back(nodeId);

    auto nextIt = straightPaths.find(nodeId);
    if (nextIt != straightPaths.end()) {
        for (int nextId : nextIt->second) {
            dfs3(loop, nextId, curPath);
        }
    }
}

void Looper::findLoopsInNormalFunc(IntermediateUnit::Function const &func) {
    std::set<int> visited;

//    for (auto &[id, loop] : loops) {
//        loop.head;
//    }

//    cfg.traverseBlocks(func.getEntryBlockId(), visited, [](int blockId) {
//
//    });
}
