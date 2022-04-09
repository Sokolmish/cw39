#include "tailrec_eliminator.hpp"
#include "cfg_cleaner.hpp"
#include <set>

TailrecEliminator::TailrecEliminator(ControlFlowGraph rawCfg) : cfg(std::move(rawCfg)) {
    for (auto &[id, func] : cfg.getFuncsMut()) {
        passFunction(func);
    }

    CfgCleaner cleaner(std::move(cfg));
    cleaner.removeNops();
    cleaner.removeUselessNodes();
    cfg = std::move(cleaner).moveCfg();
}

ControlFlowGraph const& TailrecEliminator::getCfg() {
    return cfg;
}

ControlFlowGraph TailrecEliminator::moveCfg() && {
    return std::move(cfg);
}


void TailrecEliminator::passFunction(ControlFlowGraph::Function &func) {
    std::vector<int> tailrecBlocks = findTailCalls(func);
    if (tailrecBlocks.empty())
        return;

    IR_Block &oldHead = cfg.block(func.getEntryBlockId());

    // NewHead is a dummy block, created because entry block cannot have predcessors
    IR_Block &newHead = cfg.createBlock();
    newHead.setTerminator(IR_ExprTerminator::JUMP);
    cfg.linkBlocks(newHead, oldHead);
    func.setEntryBlockId(newHead.id);

    std::vector<IRval> newArgs;
    std::vector<IR_ExprPhi *> phis;
    auto const &argsTypes = func.getFuncType()->args;

    // Place phis in oldHead (there are no other phis because it was entry block)
    for (uint64_t an = 0; auto const &argType : argsTypes) {
        IRval arg = cfg.createReg(argType);
        newArgs.push_back(arg);

        auto phi = std::make_unique<IR_ExprPhi>();
        phi->args.emplace(0, IRval::createFunArg(argType, an));
        phis.push_back(phi.get());
        oldHead.phis.emplace_back(arg, std::move(phi));

        an++;
    }

    // Remove tail calls
    for (int i = 1; int tailId : tailrecBlocks) {
        IR_Block &tailBlock = cfg.block(tailId);
        tailBlock.setTerminator(IR_ExprTerminator::JUMP);
        cfg.linkBlocks(tailBlock, oldHead);

        auto &lastNode = tailBlock.body.back();
        auto const &callExpr = dynamic_cast<IR_ExprCall const &>(*lastNode.body);
        for (uint64_t an = 0; an < argsTypes.size(); an++) {
            phis[an]->args.emplace(i, callExpr.args[an].copy());
        }
        lastNode = IR_Node::nop();

        i++;
    }

    replaceParams(func.getEntryBlockId(), newArgs);
}

std::vector<int> TailrecEliminator::findTailCalls(ControlFlowGraph::Function const &func) {
    std::set<int> visited;
    std::vector<int> tailrecBlocks;
    int funcId = func.getId();

    auto visitor = [this, funcId, &tailrecBlocks](int blockId) {
        IR_Block &curBlock = cfg.block(blockId);

        // Inspect only blocks with return statement
        auto terminator = curBlock.getTerminator();
        if (terminator->termType != IR_ExprTerminator::RET)
            return;

        if (curBlock.body.empty())
            return;

        // TODO: check if can reorder
        auto const &lastNode = curBlock.body.back();
        if (!lastNode.body || lastNode.body->type != IR_Expr::CALL)
            return;

        auto const &callExpr = dynamic_cast<IR_ExprCall const &>(*lastNode.body);
        if (callExpr.isIndirect() || callExpr.getFuncId() != funcId)
            return;

        // Check if returns result of recursive call or nothing
        if (terminator->arg.has_value()) {
            if (!lastNode.res.has_value()) // TODO: also can be tailcall
                return;
            if (!lastNode.res->equal(*terminator->arg))
                return;
        }

        tailrecBlocks.push_back(blockId);
    };

    cfg.traverseBlocks(func.getEntryBlockId(), visited, visitor);
    return tailrecBlocks;
}

void TailrecEliminator::replaceParams(int entryId, std::vector<IRval> const &newArgs) {
    std::set<int> visited;
    auto visitor = [this, &newArgs](int blockId) {
        IR_Block &block = cfg.block(blockId);
        for (IR_Node &node : block.body) {
            if (node.body) {
                for (auto arg: node.body->getArgs()) {
                    if (arg->getValueClass() == IRval::FUN_PARAM)
                        *arg = newArgs.at(arg->castValTo<int>());
                }
            }
        }
    };
    cfg.traverseBlocks(entryId, visited, visitor);
}
