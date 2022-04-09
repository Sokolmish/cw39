#ifndef CFG_CLEANER_HPP_INCLUDED__
#define CFG_CLEANER_HPP_INCLUDED__

#include "ir_transformer.hpp"
#include <set>

class CfgCleaner : IRTransformer {
private:
    ControlFlowGraph cfg;

    /** Do DFS from given block and mark all blocks, that can be accessed only trough it */
    std::set<int> getDominatedByGiven(int startId);

public:
    explicit CfgCleaner(ControlFlowGraph rawCfg);

    void removeNops();
    void fixVersions();
    void removeUselessNodes();
    void removeTransitBlocks();
    void removeUnreachableBlocks();

    ControlFlowGraph const& getCfg() override;
    ControlFlowGraph moveCfg() && override;
};

#endif /* CFG_CLEANER_HPP_INCLUDED__ */
