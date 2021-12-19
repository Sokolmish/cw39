#ifndef __COPY_PROPAGATOR_HPP__
#define __COPY_PROPAGATOR_HPP__

#include <memory>
#include <map>
#include <set>
#include <functional>
#include "ir/cfg.hpp"

class CopyPropagator {
private:
    std::shared_ptr<ControlFlowGraph> cfg;

    std::map<IRval, IRval, IRval::ComparatorVersions> remlacementMap;
    bool changed;

    std::set<int> visitedBlocks;
    void traverseBlocks(int blockId, std::function<void(int)> action);

public:
    CopyPropagator(std::shared_ptr<ControlFlowGraph> rawCfg);
    std::shared_ptr<ControlFlowGraph> getCfg();
};


#endif /* __COPY_PROPAGATOR_HPP__ */
