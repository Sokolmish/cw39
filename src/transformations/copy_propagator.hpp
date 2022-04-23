#ifndef COPY_PROPAGATOR_HPP_INCLUDED__
#define COPY_PROPAGATOR_HPP_INCLUDED__

#include "ir_transformer.hpp"
#include <map>

class CopyPropagator : IRTransformer {
public:
    explicit CopyPropagator(CFGraph rawCfg);

    CFGraph const& getCfg() override;
    CFGraph moveCfg() && override;

    bool isChanged() const;

private:
    CFGraph cfg;

    std::map<IRval, IRval> remlacementMap;
    bool changed, globalChanged;

    void propagateCopies();
    void foldConstants();

    IRval doConstOperation(IR_ExprOper const &oper);
};

#endif /* COPY_PROPAGATOR_HPP_INCLUDED__ */
