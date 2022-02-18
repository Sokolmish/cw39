#ifndef ALGEBRAIC_TRANSFORMER_HPP_INCLUDED__
#define ALGEBRAIC_TRANSFORMER_HPP_INCLUDED__

#include <memory>
#include "ir/cfg.hpp"

class AlgebraicTransformer {
private:
    std::shared_ptr<ControlFlowGraph> cfg;

    void processNode(IR_Node *node);

public:
    explicit AlgebraicTransformer(std::shared_ptr<ControlFlowGraph> const &rawCfg);
    std::shared_ptr<ControlFlowGraph> getCfg();
};

#endif /* ALGEBRAIC_TRANSFORMER_HPP_INCLUDED__ */
