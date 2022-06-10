#ifndef GLOBAL_TRANSFORMERS_HPP_INCLUDED__
#define GLOBAL_TRANSFORMERS_HPP_INCLUDED__

#include "ir_transformer.hpp"
#include <vector>

class TailrecEliminator : public IRTransformer {
public:
    TailrecEliminator(IntermediateUnit const &unit, CFGraph rawCfg, int funcId);

private:
    IntermediateUnit const *iunit;

    void passFunction(int funcId);
    std::vector<int> findTailCalls(int funcId);
    void replaceParams(int entryId, const std::vector<IRval> &newArgs);
};

class FunctionsInliner : public IRTransformer {
public:
    FunctionsInliner(IntermediateUnit const &unit, CFGraph rawCfg);

private:
    IntermediateUnit const *iunit;

    bool passBlock(IR_Block &block);
    IR_Block& inlineFunc(IntermediateUnit::Function const &func, IR_Block &retBlock, IR_Node const &callingNode);
    void reenumerateRegisters(std::vector<IR_Block*> const &blocks, IR_Node const &callingNode);
};

#endif /* GLOBAL_TRANSFORMERS_HPP_INCLUDED__ */
