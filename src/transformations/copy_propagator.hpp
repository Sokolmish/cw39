#ifndef COPY_PROPAGATOR_HPP_INCLUDED__
#define COPY_PROPAGATOR_HPP_INCLUDED__

#include "ir_transformer.hpp"
#include <map>

class CopyPropagator : public IRTransformer {
public:
    explicit CopyPropagator(CFGraph rawCfg);

private:
    std::map<IRval, IRval> remlacementMap;
    bool changed;

    void propagateCopies();
    void foldConstants();

    IRval doConstOperation(IR_ExprOper const &oper);
};

#endif /* COPY_PROPAGATOR_HPP_INCLUDED__ */
