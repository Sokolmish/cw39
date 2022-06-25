#include "intrinsics_detector.hpp"
#include "cfg_cleaner.hpp"
#include "loop_inv_mover.hpp"
#include <stack>

IntrinsicsDetector::IntrinsicsDetector(IntermediateUnit const &unit, CFGraph rawCfg)
        : IRTransformer(std::move(rawCfg)), loops(cfg) {
    std::vector<LoopNode const*> leafLoops;
    for (auto const &[lId, loop] : loops.getLoops()) {
        if (loop.children.empty())
            leafLoops.push_back(&loop);
    }
    for (LoopNode const *loop : leafLoops) {
        passLoop(*loop);
    }

    cfg = LoopInvMover(unit, std::move(cfg)).moveCfg();
    CfgCleaner cleaner(unit, std::move(cfg));
    cleaner.removeUselessLoops();
    cleaner.removeUselessNodes();
    cleaner.removeNops();
    cleaner.removeUselessBranches();
    cleaner.removeUnreachableBlocks();
    cleaner.removeTransitBlocks();
    if (cleaner.isPassEffective())
        setPassChanged();
    cfg = std::move(cleaner).moveCfg();
}

void IntrinsicsDetector::passLoop(LoopNode const &loop) {
    if (!hasNoExitsInside(loop))
        return;

    // TODO: many inner and outer head predcessors with only two values (concentrating blocks)
    if (loop.head.prev.size() != 2)
        return;

    IR_Block &head = cfg.block(loop.head.id);

    int outPredNum = 0;
    int inPredNum = 1;
    int outArcId = head.prev.at(outPredNum);
    int inArcId = head.prev.at(inPredNum);
    if (loop.blocks.contains(outArcId) && !loop.blocks.contains(inArcId)) {
        std::swap(inArcId, outArcId);
        outArcId = head.prev.at(outPredNum);
        inArcId = head.prev.at(inPredNum);
    }
    if (loop.blocks.contains(outArcId) || !loop.blocks.contains(inArcId))
        throw cw39_internal_error("Something wrong with loop structure");

    std::optional<IRval> ctzArg;
    std::optional<IRval> oldCtzRes;

    for (IR_Node const &phi : loop.head.phis) {
        IRval const &cntInit = phi.body->getPhi().args.at(outPredNum);
        IRval const &cntInductive = phi.body->getPhi().args.at(inPredNum);
        if (cntInit.isConstant() && cntInit.castValTo<uint64_t>() == 0) {
            if (!cntInductive.isVReg()) // Additional early check
                continue;
            auto examCtzRes = examForCTZ(loop, phi, cntInductive);
            if (examCtzRes) {
                ctzArg = **examCtzRes;
                oldCtzRes = phi.res.value();
                break;
            }
        }
    }

    if (!ctzArg || !oldCtzRes)
        return;

    // Still assume that we have only 1 exit from loop (in header)
    int outerSuccId = -1;
    for (int next : loop.head.next) {
        if (!loop.blocks.contains(next)) {
            outerSuccId = next;
            break;
        }
    }
    if (outerSuccId == -1)
        throw cw39_internal_error("Something wrong with loop structure");

    IRval newCtzRes = cfg.createReg(oldCtzRes->getType());
    head.addNode(newCtzRes, std::make_unique<IR_ExprOper>(
            IR_ExprOper::INTR_CTZ, std::vector<IRval>{ *ctzArg }));

    auto visitor = [this, &oldCtzRes, &newCtzRes](int blockId) {
        IR_Block &block = cfg.block(blockId);
        for (IR_Node *node : block.getAllNodes()) {
            for (IRval *arg : node->body->getArgs()) {
                if (*arg == oldCtzRes) {
                    *arg = newCtzRes;
                }
            }
        }
    };
    std::set<int> visited;
    visited.insert(head.id); // Prevent entering into loop
    cfg.traverseBlocks(outerSuccId, visited, visitor);

    setPassChanged();
}

struct BinCommutativeArgs {
    IRval const *constVal;
    IRval const *varVal;
};

/** Checks, if commutative operation has 1 constant and 1 variable arguments and returns them */
static std::optional<BinCommutativeArgs> splitBinCommArgs(IR_ExprOper const &expr) {
    if (expr.args.at(0).isConstant() && expr.args.at(1).isVReg()) {
        return (BinCommutativeArgs) {
                .constVal = &expr.args.at(0),
                .varVal = &expr.args.at(1),
        };
    }
    else if (expr.args.at(1).isConstant() && expr.args.at(0).isVReg()) {
        return (BinCommutativeArgs) {
                .constVal = &expr.args.at(1),
                .varVal = &expr.args.at(0),
        };
    }
    else {
        return {};
    }
}

std::optional<IRval const*>
IntrinsicsDetector::examForCTZ(LoopNode const &loop, IR_Node const &phi, IRval const &cntInductive) {
    enum {
        STEP_SHIFT, STEP_AND1, STEP_EQ0, STEP_BRANCH
    } chainStep = STEP_SHIFT;
    IRval const &counterVal = phi.res.value();
    IRval const *argVal = nullptr;
    IRval const *shiftedVal = nullptr;
    IRval const *bitVal = nullptr;
    IRval const *testVal = nullptr;

    for (IR_Node const &node : loop.head.body) {
        auto oper = dynamic_cast<IR_ExprOper const *>(node.body.get());
        if (!oper) // All necessary nodes are Operations
            continue;

        if (chainStep == STEP_SHIFT) {
            if (oper->op != IR_ExprOper::SHR)
                continue;
            if (oper->args.at(1) != counterVal)
                continue;

            argVal = &oper->args.at(0);
            shiftedVal = &node.res.value();
            chainStep = STEP_AND1;
        }
        else if (chainStep == STEP_AND1) {
            if (oper->op != IR_ExprOper::AND)
                continue;

            auto commArgs = splitBinCommArgs(*oper);
            if (!commArgs)
                continue;
            if (*commArgs->varVal != *shiftedVal)
                continue;
            if (commArgs->constVal->castValTo<uint64_t>() != 1)
                continue;

            bitVal = &node.res.value();
            chainStep = STEP_EQ0;
        }
        else { // chainStep == STEP_EQ0
            if (oper->op != IR_ExprOper::EQ) // TODO: inverted, also can be GT, ...
                continue;

            auto commArgs = splitBinCommArgs(*oper);
            if (!commArgs)
                continue;
            if (*commArgs->varVal != *bitVal)
                continue;
            if (commArgs->constVal->castValTo<uint64_t>() != 0)
                continue;

            testVal = &node.res.value();
            chainStep = STEP_BRANCH;
            break;
        }
    }

    if (chainStep != STEP_BRANCH)
        return {};

    auto const &term = loop.head.termNode.value().body->getTerm();
    if (term.termType != IR_ExprTerminator::BRANCH || term.arg != *testVal)
        return {};

    // Check, that if tst==0, control leaves loop
    // TODO: only with EQ0
    if (!loop.blocks.contains(loop.head.next.at(0)) || loop.blocks.contains(loop.head.next.at(1)))
        return {};

    // Chech inductive phi's arg
    bool inductiveCorrect = false;
    auto visitor = [&loop, &cntInductive, &inductiveCorrect, &counterVal](IR_Block &block) -> bool {
        if (block.id == loop.head.id)
            return true; // Skip head block

        IR_Node const *inductiveNode = nullptr;
        for (IR_Node const *node : block.getAllNodes()) {
            if (node->res == cntInductive) {
                inductiveNode = node;
                break;
            }
        }

        if (!inductiveNode)
            return true; // Not in this block

        auto oper = dynamic_cast<IR_ExprOper const *>(inductiveNode->body.get());
        if (!oper || oper->op != IR_ExprOper::ADD)
            return false;

        auto commArgs = splitBinCommArgs(*oper);
        if (!commArgs)
            return false;
        if (*commArgs->varVal != counterVal)
            return false;
        if (commArgs->constVal->castValTo<uint64_t>() != 1)
            return false;

        inductiveCorrect = true;
        return false;
    };
    loops.traverseLoop(loop, visitor);

    if (inductiveCorrect)
        return argVal;
    else
        return {};
}

/** Check, if loop has only one exit in the header */
bool IntrinsicsDetector::hasNoExitsInside(LoopNode const &loop) const {
    for (int blockId : loop.blocks) {
        if (blockId == loop.head.id)
            continue;
        IR_Block const &block = cfg.block(blockId);
        for (int nextId : block.next) {
            if (!loop.blocks.contains(nextId))
                return false;
        }
    }
    return true;
}
