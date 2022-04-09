#ifndef IR_TRANSFORMER_HPP_INCLUDED__
#define IR_TRANSFORMER_HPP_INCLUDED__

#include "ir/cfg.hpp"

class IRTransformer {
public:
    virtual ControlFlowGraph const& getCfg() = 0;
    virtual ControlFlowGraph moveCfg() && = 0;
};

#endif /* IR_TRANSFORMER_HPP_INCLUDED__ */
