#ifndef TAILREC_ELIMINATOR_HPP_INCLUDED__
#define TAILREC_ELIMINATOR_HPP_INCLUDED__

#include <memory>
#include <map>
#include <optional>
#include <functional>
#include "ir/cfg.hpp"

class TailrecEliminator {
public:
    explicit TailrecEliminator(std::shared_ptr<ControlFlowGraph> const &rawCfg);
    std::shared_ptr<ControlFlowGraph> getCfg();

private:
    std::shared_ptr<ControlFlowGraph> cfg;

    void passFunction(ControlFlowGraph::Function &func);
    std::vector<int> findTailCalls(ControlFlowGraph::Function const &func) const;
    void replaceParams(int entryId, const std::vector<IRval> &newArgs);
};


#endif /* TAILREC_ELIMINATOR_HPP_INCLUDED__ */
