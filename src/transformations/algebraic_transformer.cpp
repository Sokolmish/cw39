#include "algebraic_transformer.hpp"
#include "utils.hpp"

AlgebraicTransformer::AlgebraicTransformer(std::shared_ptr<ControlFlowGraph> rawCfg)
        : cfg(std::make_shared<ControlFlowGraph>(*rawCfg)) {
    std::set<int> visited;
    for (auto const &[fId, func] : cfg->getFuncs()) {
        cfg->traverseBlocks(func.getEntryBlockId(), visited, [this](int blockId) {
            auto &curBlock = cfg->block(blockId);
            for (auto *node : curBlock.getAllNodes()) {
                processNode(node);
            }
        });
    }
}

static bool isZero(IRval const &val) {
    if (val.getType()->type != IR_Type::DIRECT)
        return false;
    auto dirType = std::dynamic_pointer_cast<IR_TypeDirect>(val.getType());
    if (!dirType->isInteger())
        return false;
    else
        return val.castValTo<uint64_t>() == 0ULL;
}

void AlgebraicTransformer::processNode(IR_Node *node) {
    if (node->body->type != IR_Expr::OPERATION)
        return;
    auto &oper = dynamic_cast<IR_ExprOper &>(*node->body);

    if (isInList(oper.op, { IR_ADD, IR_SUB })) {
        if (isZero(oper.args[0])) {
            if (oper.op == IR_ADD) {
                oper.op = IR_MOV;
                oper.args = std::vector<IRval>{ oper.args[1] };
            }
        }
        else if (isZero(oper.args[1])) {
            oper.op = IR_MOV;
            oper.args = std::vector<IRval>{ oper.args[0] };
        }
    }
    else if (isInList(oper.op, { IR_MUL })) {
        if (isZero(oper.args[0])) {
            oper.op = IR_MOV;
            oper.args = std::vector<IRval>{ oper.args[0] };
        }
        else if (isZero(oper.args[1])) {
            oper.op = IR_MOV;
            oper.args = std::vector<IRval>{ oper.args[1] };
        }
    }
    else if (isInList(oper.op, { IR_DIV, IR_REM })) {
        if (isZero(oper.args[0])) {
            oper.op = IR_MOV;
            oper.args = std::vector<IRval>{ oper.args[0] };
        }
        else {
            semanticError("Division by zero");
        }
    }
}

std::shared_ptr<ControlFlowGraph> AlgebraicTransformer::getCfg() {
    return cfg;
}
