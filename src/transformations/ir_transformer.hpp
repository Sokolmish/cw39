#ifndef IR_TRANSFORMER_HPP_INCLUDED__
#define IR_TRANSFORMER_HPP_INCLUDED__

#include "ir/unit.hpp"

class IRTransformer {
public:
    virtual CFGraph const& getCfg() = 0;
    virtual CFGraph moveCfg() && = 0;
};

#endif /* IR_TRANSFORMER_HPP_INCLUDED__ */
