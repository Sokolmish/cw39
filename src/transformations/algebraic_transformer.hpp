#ifndef ALGEBRAIC_TRANSFORMER_HPP_INCLUDED__
#define ALGEBRAIC_TRANSFORMER_HPP_INCLUDED__

#include "ir_transformer.hpp"

class AlgebraicTransformer : IRTransformer {
public:
    explicit AlgebraicTransformer(CFGraph rawCfg);

    CFGraph const& getCfg() override;
    CFGraph moveCfg() && override;

    bool isChanged() const;

private:
    CFGraph cfg;
    bool changed = false;

    void processNode(IR_Node *node);
};

#endif /* ALGEBRAIC_TRANSFORMER_HPP_INCLUDED__ */
