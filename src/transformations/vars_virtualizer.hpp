#ifndef VARS_VIRTUALIZER_HPP_INCLUDED__
#define VARS_VIRTUALIZER_HPP_INCLUDED__

#include <map>
#include <optional>
#include "ir_transformer.hpp"

// TODO: check for phi functions

class VarsVirtualizer : IRTransformer {
public:
    explicit VarsVirtualizer(CFGraph rawCfg);

    CFGraph const& getCfg() override;
    CFGraph moveCfg() && override;

private:
    CFGraph cfg;

    std::map<IRval, std::optional<IRval>, IRval::Comparator> toRedudeList;

    void passFunction();
    void analyzeBlock(IR_Block const &block);
    void optimizeBlock(IR_Block &block);
};

#endif /* VARS_VIRTUALIZER_HPP_INCLUDED__ */
