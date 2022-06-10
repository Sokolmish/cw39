#ifndef INTRINSICS_DETECTOR_HPP_INCLUDED__
#define INTRINSICS_DETECTOR_HPP_INCLUDED__

#include "ir_transformer.hpp"
#include "graph_info.hpp"

class IntrinsicsDetector : public IRTransformer {
public:
    IntrinsicsDetector(IntermediateUnit const &unit, CFGraph rawCfg);

private:
    LoopsDetector loops;

    using LoopNode = LoopsDetector::LoopNode;

    void passLoop(LoopNode const &loop);
    bool hasNoExitsInside(LoopNode const &loop) const;
    std::optional<IRval const*> examForCTZ(LoopNode const &loop, IR_Node const &phi, IRval const &cntInductive);
};

#endif /* INTRINSICS_DETECTOR_HPP_INCLUDED__ */
