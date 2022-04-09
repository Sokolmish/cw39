#ifndef TAILREC_ELIMINATOR_HPP_INCLUDED__
#define TAILREC_ELIMINATOR_HPP_INCLUDED__

#include <vector>
#include "ir_transformer.hpp"

class TailrecEliminator :IRTransformer {
public:
    explicit TailrecEliminator(ControlFlowGraph rawCfg);

    ControlFlowGraph const& getCfg() override;
    ControlFlowGraph moveCfg() && override;

private:
    ControlFlowGraph cfg;

    void passFunction(ControlFlowGraph::Function &func);
    std::vector<int> findTailCalls(ControlFlowGraph::Function const &func);
    void replaceParams(int entryId, const std::vector<IRval> &newArgs);
};

#endif /* TAILREC_ELIMINATOR_HPP_INCLUDED__ */
