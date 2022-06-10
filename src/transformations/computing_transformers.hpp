#ifndef COMPUTING_TRANSFORMERS_HPP_INCLUDED__
#define COMPUTING_TRANSFORMERS_HPP_INCLUDED__

#include "ir_transformer.hpp"

class CopyPropagator : public IRTransformer {
public:
    explicit CopyPropagator(IntermediateUnit const &iunit, CFGraph rawCfg);

private:
    std::map<IRval, IRval> remlacementMap;
    bool changed;

    void propagateCopies();
    void foldConstants();
};

class AlgebraicTransformer : public IRTransformer {
public:
    explicit AlgebraicTransformer(IntermediateUnit const &unit, CFGraph rawCfg);

private:
    void processNode(IR_Node *node);
};

class CommonSubexprElim : public IRTransformer {
public:
    CommonSubexprElim(IntermediateUnit const &iunit, CFGraph rawCfg);
};

#endif /* COMPUTING_TRANSFORMERS_HPP_INCLUDED__ */
