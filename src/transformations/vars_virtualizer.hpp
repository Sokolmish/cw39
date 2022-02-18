#ifndef VARS_VIRTUALIZER_HPP_INCLUDED__
#define VARS_VIRTUALIZER_HPP_INCLUDED__

#include <memory>
#include <map>
#include <optional>
#include <functional>
#include "ir/cfg.hpp"

// TODO: check for phi functions

class VarsVirtualizer {
private:
    std::shared_ptr<ControlFlowGraph> cfg;

    std::map<IRval, std::optional<IRval>, IRval::ComparatorIgnoreVers> toRedudeList;

    void passFunction(ControlFlowGraph::Function const &func);
    void analyzeBlock(IR_Block const &block);
    void optimizeBlock(IR_Block &block);

public:
    explicit VarsVirtualizer(ControlFlowGraph const &rawCfg);
    std::shared_ptr<ControlFlowGraph> getCfg();
};

#endif /* VARS_VIRTUALIZER_HPP_INCLUDED__ */
