#ifndef VARS_VIRTUALIZER_HPP_INCLUDED__
#define VARS_VIRTUALIZER_HPP_INCLUDED__

#include <map>
#include <optional>
#include "ir_transformer.hpp"

// TODO: check for phi functions

class VarsVirtualizer : public IRTransformer {
public:
    explicit VarsVirtualizer(CFGraph rawCfg);

private:
    std::map<IRval, std::optional<IRval>> toRedudeList;

    void passFunction();
    void analyzeBlock(IR_Block const &block);
    void optimizeBlock(IR_Block &block);
};

#endif /* VARS_VIRTUALIZER_HPP_INCLUDED__ */
