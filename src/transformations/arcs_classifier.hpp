#ifndef __ARCS_CLASSIFIER_HPP__
#define __ARCS_CLASSIFIER_HPP__

#include <map>
#include <string>
#include <memory>
#include "ir/cfg.hpp"

// TODO: integrate with dominators
class ArcsClassifier {
public:
    explicit ArcsClassifier(std::shared_ptr<ControlFlowGraph> in_cfg);

    enum ArcClass { NONE, TREE, BACK, FORWARD, CROSS };

    ArcClass getArcClass(int from, int to);

    std::string drawGraph() const;

private:
    std::shared_ptr<ControlFlowGraph> cfg;
    std::map<std::pair<int, int>, ArcClass> arcsClasses;

    int globalTime = 1;

    struct NodeTimes {
        int in, out;
    };
    std::map<int, NodeTimes> times;

    void dfs(int blockId, NodeTimes &blockTimes);
};

#endif /* __ARCS_CLASSIFIER_HPP__ */
