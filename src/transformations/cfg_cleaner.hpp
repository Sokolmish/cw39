#ifndef __CFG_CLEANER_HPP__
#define __CFG_CLEANER_HPP__

#include "ir/cfg.hpp"
#include <map>
#include <set>
#include <functional>

class CfgCleaner {
private:
    std::shared_ptr<ControlFlowGraph> cfg;

    /** Do DFS from given block and mark all blocks, that can be accessed only trough it */
    std::set<int> getDominatedByGiven(int startId);

public:
    explicit CfgCleaner(std::shared_ptr<ControlFlowGraph> const &rawCfg);

    void removeNops();
    void fixVersions();
    void removeUselessNodes();
    void removeTransitBlocks();
    void removeUnreachableBlocks();

    std::shared_ptr<ControlFlowGraph> getCfg();
};


#endif /* __CFG_CLEANER_HPP__ */
