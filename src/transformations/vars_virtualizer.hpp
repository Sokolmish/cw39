#ifndef __VARS_VIRTUALIZER_HPP__
#define __VARS_VIRTUALIZER_HPP__

#include <memory>
#include <map>
#include <optional>
#include <functional>
#include "ir/cfg.hpp"

class VarsVirtualizer {
private:
    std::shared_ptr<ControlFlowGraph> cfg;

    std::map<IRval, std::optional<IRval>, IRval::Comparator> toRedudeList;

    void passFunction(ControlFlowGraph::Function const &func);
    void analyzeBlock(IR_Block const &block);
    void optimizeBlock(IR_Block &block);

public:
    explicit VarsVirtualizer(ControlFlowGraph rawCfg);
    std::shared_ptr<ControlFlowGraph> getCfg();
};

#endif /* __VARS_VIRTUALIZER_HPP__ */
