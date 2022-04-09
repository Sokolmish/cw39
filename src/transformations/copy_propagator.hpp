#ifndef COPY_PROPAGATOR_HPP_INCLUDED__
#define COPY_PROPAGATOR_HPP_INCLUDED__

#include "ir_transformer.hpp"
#include <map>

class CopyPropagator : IRTransformer {
private:
    ControlFlowGraph cfg;

    std::map<IRval, IRval, IRval::Comparator> remlacementMap;
    bool changed, globalChanged;

    void propagateCopies();
    void foldConstants();

    IRval doConstOperation(IR_ExprOper const &oper);

public:
    explicit CopyPropagator(ControlFlowGraph rawCfg);

    ControlFlowGraph const& getCfg() override;
    ControlFlowGraph moveCfg() && override;
};

#endif /* COPY_PROPAGATOR_HPP_INCLUDED__ */
