#ifndef __CFG_CLEANER_HPP__
#define __CFG_CLEANER_HPP__

#include "ir/cfg.hpp"
#include <map>
#include <set>
#include <functional>

class CfgCleaner {
private:
    std::shared_ptr<ControlFlowGraph> cfg;

    std::set<int> visitedBlocks;
    void traverseBlocks(int blockId, std::function<void(int blockId)> action);

public:
    CfgCleaner(std::shared_ptr<ControlFlowGraph> rawCfg);

    void removeNops();
    void fixVersions();
    void removeUselessNodes();

    std::shared_ptr<ControlFlowGraph> getCfg();
};


#endif /* __CFG_CLEANER_HPP__ */
