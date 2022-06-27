#ifndef CFG_CLEANER_HPP_INCLUDED__
#define CFG_CLEANER_HPP_INCLUDED__

#include "ir_transformer.hpp"
#include "graph_info.hpp"
#include <set>

class CfgCleaner : public IRTransformer {
public:
    explicit CfgCleaner(IntermediateUnit const &unit, CFGraph rawCfg);

    void removeNops();
    void removeUselessNodes(); // Makes deep clean. Maybe also create fast clean?
    void removeTransitBlocks();
    void removeUselessBranches();
    void removeUnreachableBlocks();
    void removeUselessLoops();

private:
    IntermediateUnit const *iunit;

    std::set<IRval> getPrimaryEffectiveRegs();
    std::set<IRval> extendEffectiveRegsSet(std::set<IRval> const &regsSet);
    void removeUnusedNodes(std::set<IRval> const &usedRegs);

    /** Do DFS from given block and mark all blocks, that can be accessed only trough it */
    std::set<int> getDominatedByGiven(int startId);

    using LoopNode = LoopsDetector::LoopNode;
    bool isNodeGeneralEffective(IR_Expr const &node);
    bool isLoopEffective(LoopNode const &loop);
    std::pair<int, int> getLoopExit(LoopNode const &loop);
};

#endif /* CFG_CLEANER_HPP_INCLUDED__ */
