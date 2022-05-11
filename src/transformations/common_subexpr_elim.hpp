#ifndef COMMON_SUBEXPR_ELIM_HPP_INCLUDED__
#define COMMON_SUBEXPR_ELIM_HPP_INCLUDED__

#include "ir_transformer.hpp"

class CommonSubexprElim : public IRTransformer {
public:
    CommonSubexprElim(CFGraph rawCfg);
};

#endif /* COMMON_SUBEXPR_ELIM_HPP_INCLUDED__ */
