#include "arcs_classifier.hpp"
#include "utils.hpp"
#include <sstream>
#include <fmt/ostream.h>

ArcsClassifier::ArcsClassifier(std::shared_ptr<ControlFlowGraph> in_cfg) : cfg(std::move(in_cfg)) {
    for (auto const &[blockId, block] : cfg->getBlocks()) {
        times.emplace(blockId, NodeTimes{ -1, -1 });
    }

    for (auto const &[fid, func] : cfg->getFuncs()) {
        int entryId = func.getEntryBlockId();
        dfs(entryId, times.at(entryId));
    }

    // All arcs are in arcsClasses and marked as TREE or NONE
    for (auto &[arc, cl] : arcsClasses) {
        if (cl != ArcsClassifier::NONE)
            continue;

        auto const &from = times.at(arc.first);
        auto const &to = times.at(arc.second);

        if (from.in < to.in && to.out < from.out) // Possible simplify conditions?
            cl = ArcsClassifier::FORWARD;
        else if (to.in < from.in && from.out < to.out)
            cl = ArcsClassifier::BACK;
        else if (to.out < from.in)
            cl = ArcsClassifier::CROSS;
        else
            internalError("Smth went wrong on arcs classification");
    }

    times.clear();
    cfg.reset();
}

void ArcsClassifier::dfs(int blockId, NodeTimes &blockTimes) {
    blockTimes.in = globalTime++;

    IR_Block &cur = cfg->block(blockId);
    for (int next : cur.next) {
        auto &nextTimes = times.at(next);
        if (nextTimes.in == -1) {
            arcsClasses.emplace(std::make_pair(blockId, next), ArcsClassifier::TREE);
            dfs(next, nextTimes);
        }
        else {
            // Will be classified later
            arcsClasses.emplace(std::make_pair(blockId, next), ArcsClassifier::NONE);
        }
    }

    blockTimes.out = globalTime++;
}

ArcsClassifier::ArcClass ArcsClassifier::getArcClass(int from, int to) {
    auto it = arcsClasses.find(std::make_pair(from, to));
    return it == arcsClasses.end() ? ArcsClassifier::NONE : it->second;
}

std::string ArcsClassifier::drawGraph() const {
    std::stringstream ss;
    fmt::print(ss, "digraph{{ ");
    for (auto &[arc, cl] : arcsClasses) {
        std::string format;
        switch (cl) {
            case ArcsClassifier::NONE:
                format = "{}->{}[style=\"dotted,bold\",color=purple]; ";
                break;
            case ArcsClassifier::TREE:
                format = "{}->{}[style=\"bold\",color=black]; ";
                break;
            case ArcsClassifier::BACK:
                format = "{}->{}[style=\"solid\",color=darkred]; ";
                break;
            case ArcsClassifier::CROSS:
                format = "{}->{}[style=\"dashed\",color=darkblue]; ";
                break;
            case ArcsClassifier::FORWARD:
                format = "{}->{}[style=\"dashed,bold\",color=darkgreen]; ";
                break;
            default:
                internalError("Smth went wrong %d");
        }
        fmt::print(ss, format, arc.first, arc.second);
    }
    fmt::print(ss, "}}\n");
    return ss.str();
}
