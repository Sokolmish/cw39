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

    std::map<IRval, IRval, IRval::Comparator> remlacementMap;
    bool changed, globalChanged;

    void propagateCopies();
    void foldConstants();

    IRval doConstOperation(IR_ExprOper const &oper);

public:
    explicit CopyPropagator(std::shared_ptr<ControlFlowGraph> const &rawCfg);
    std::shared_ptr<ControlFlowGraph> getCfg();
};


#endif /* __COPY_PROPAGATOR_HPP__ */
