#ifndef VARS_VIRTUALIZER_HPP_INCLUDED__
#define VARS_VIRTUALIZER_HPP_INCLUDED__

#include <map>
#include <optional>
#include "ir_transformer.hpp"

// TODO: check for phi functions

class VarsVirtualizer : IRTransformer {
private:
    ControlFlowGraph cfg;

    std::map<IRval, std::optional<IRval>, IRval::ComparatorIgnoreVers> toRedudeList;

    void passFunction(ControlFlowGraph::Function const &func);
    void analyzeBlock(IR_Block const &block);
    void optimizeBlock(IR_Block &block);

public:
    explicit VarsVirtualizer(ControlFlowGraph rawCfg);

    ControlFlowGraph const& getCfg() override;
    ControlFlowGraph moveCfg() && override;
};

#endif /* VARS_VIRTUALIZER_HPP_INCLUDED__ */
