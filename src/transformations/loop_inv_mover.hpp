#ifndef LOOP_INV_MOVER_HPP_INCLUDED__
#define LOOP_INV_MOVER_HPP_INCLUDED__

#include "ir_transformer.hpp"
#include "graph_info.hpp"

class LoopInvMover : public IRTransformer {
public:
    LoopInvMover(IntermediateUnit const &unit, CFGraph rawCfg);

private:
    LoopsDetector loops;

    using LoopNode = LoopsDetector::LoopNode;

    void passNestedLoops(LoopNode const &topLoop);
    void passLoop(LoopNode const &loop);
    void traverseLoop(LoopNode const &loop, std::function<void(int)> const &visit);
    void moveInvariantNodes(LoopNode const &loop, std::vector<IR_Node> &&invNodes);
};

#endif /* LOOP_INV_MOVER_HPP_INCLUDED__ */
