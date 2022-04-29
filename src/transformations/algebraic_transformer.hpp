#ifndef ALGEBRAIC_TRANSFORMER_HPP_INCLUDED__
#define ALGEBRAIC_TRANSFORMER_HPP_INCLUDED__

#include "ir_transformer.hpp"

class AlgebraicTransformer : public IRTransformer {
public:
    explicit AlgebraicTransformer(CFGraph rawCfg);

private:
    void processNode(IR_Node *node);
};

#endif /* ALGEBRAIC_TRANSFORMER_HPP_INCLUDED__ */
