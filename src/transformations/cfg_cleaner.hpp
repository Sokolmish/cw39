#ifndef CFG_CLEANER_HPP_INCLUDED__
#define CFG_CLEANER_HPP_INCLUDED__

#include "ir_transformer.hpp"
#include <set>

class CfgCleaner : public IRTransformer {
public:
    explicit CfgCleaner(CFGraph rawCfg);

    void removeNops();
    void removeUselessNodes(); // Makes deep clean. Maybe also create fast clean?
    void removeTransitBlocks();
    void removeUselessBranches();
    void removeUnreachableBlocks();

private:
    std::set<IRval> getPrimaryEffectiveRegs();
    std::set<IRval> extendEffectiveRegsSet(std::set<IRval> const &regsSet);
    void removeUnusedNodes(std::set<IRval> const &usedRegs);

    /** Do DFS from given block and mark all blocks, that can be accessed only trough it */
    std::set<int> getDominatedByGiven(int startId);
};

#endif /* CFG_CLEANER_HPP_INCLUDED__ */
