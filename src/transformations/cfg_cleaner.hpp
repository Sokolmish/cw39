#ifndef CFG_CLEANER_HPP_INCLUDED__
#define CFG_CLEANER_HPP_INCLUDED__

#include "ir_transformer.hpp"
#include <set>

class CfgCleaner : IRTransformer {
public:
    explicit CfgCleaner(CFGraph rawCfg);

    void removeNops();
    void removeUselessNodes(); // Makes deep clean. Maybe also create fast clean?
    void removeTransitBlocks();
    void removeUnreachableBlocks();

    CFGraph const& getCfg() override;
    CFGraph moveCfg() && override;

private:
    CFGraph cfg;

    using RegsSet_t = std::set<IRval, IRval::Comparator>;
    RegsSet_t getPrimaryEffectiveRegs();
    RegsSet_t extendEffectiveRegsSet(RegsSet_t const &regsSet);
    void removeUnusedNodes(RegsSet_t const &usedRegs);

    /** Do DFS from given block and mark all blocks, that can be accessed only trough it */
    std::set<int> getDominatedByGiven(int startId);
};

#endif /* CFG_CLEANER_HPP_INCLUDED__ */
