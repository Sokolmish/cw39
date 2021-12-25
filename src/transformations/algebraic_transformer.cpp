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

// /**
// * Returns log2(v) if it is whole or -1 otherwise
// * http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
// */
//static int getTwosPower(int v) {
//    if (!v)
//        return 0;
//    if ((v & (v - 1)) != 0)
//        return -1;
//    static const unsigned int b[] = {
//            0xAAAAAAAA, 0xCCCCCCCC, 0xF0F0F0F0, 0xFF00FF00, 0xFFFF0000 };
//    unsigned int r = (v & b[0]) != 0;
//    for (int i = 4; i > 0; i--)
//        r |= ((v & b[i]) != 0) << i;
//    return r;
//}

static bool isConstEqual(IRval const &ir_val, uint64_t num_val) {
    if (!ir_val.isConstant())
        return false;
    if (ir_val.getType()->type != IR_Type::DIRECT)
        return false;
    auto dirType = std::dynamic_pointer_cast<IR_TypeDirect>(ir_val.getType());
    if (!dirType->isInteger())
        return false;
    else
        return ir_val.castValTo<uint64_t>() == num_val;
}

void AlgebraicTransformer::processNode(IR_Node *node) {
    if (node->body->type != IR_Expr::OPERATION)
        return;
    auto &oper = dynamic_cast<IR_ExprOper &>(*node->body);

    // Add or subtract 0
    if (isInList(oper.op, { IR_ADD, IR_SUB })) {
        if (isConstEqual(oper.args[0], 0ULL)) {
            if (oper.op == IR_ADD) {
                oper.op = IR_MOV;
                oper.args = std::vector<IRval>{ oper.args[1] };
            }
        }
        else if (isConstEqual(oper.args[1], 0ULL)) {
            oper.op = IR_MOV;
            oper.args = std::vector<IRval>{ oper.args[0] };
        }
    }

    // Mul by 0
    if (isInList(oper.op, { IR_MUL })) {
        if (isConstEqual(oper.args[0], 0ULL)) {
            oper.op = IR_MOV;
            oper.args = std::vector<IRval>{ oper.args[0] };
        }
        else if (isConstEqual(oper.args[1], 0ULL)) {
            oper.op = IR_MOV;
            oper.args = std::vector<IRval>{ oper.args[1] };
        }
    }

    // Zero division
    if (isInList(oper.op, { IR_DIV, IR_REM })) {
        if (isConstEqual(oper.args[0], 0ULL)) {
            oper.op = IR_MOV;
            oper.args = std::vector<IRval>{ oper.args[0] };
        }
        else if (isConstEqual(oper.args[1], 0ULL)) {
            semanticError("Division by zero");
        }
    }

    // Bit shift of 0 and by 0
    if (isInList(oper.op, { IR_SHR, IR_SHL })) {
        if (isConstEqual(oper.args[1], 0ULL)) {
            oper.op = IR_MOV;
            oper.args = std::vector<IRval>{ oper.args[0] };
        }
        else if (isConstEqual(oper.args[0], 0ULL)) {
            oper.op = IR_MOV;
            oper.args = std::vector<IRval>{ oper.args[0] };
        }
    }

    // Mul by 1
    if (isInList(oper.op, { IR_MUL })) {
        if (isConstEqual(oper.args[1], 1ULL)) {
            oper.op = IR_MOV;
            oper.args = std::vector<IRval>{ oper.args[0] };
        }
        else if (isConstEqual(oper.args[0], 1ULL)) {
            oper.op = IR_MOV;
            oper.args = std::vector<IRval>{ oper.args[1] };
        }
    }

    // Div by 1
    if (isInList(oper.op, { IR_MUL })) {
        if (isConstEqual(oper.args[1], 1ULL)) {
            oper.op = IR_MOV;
            oper.args = std::vector<IRval>{ oper.args[0] };
        }
    }

    // Zero subtraction
    if (isInList(oper.op, { IR_SUB, IR_XOR })) {
        // TODO: see note at comparsion operator
        if (oper.args[0] == oper.args[1] && oper.args[0].version == oper.args[1].version) {
            oper.op = IR_MOV;
            oper.args = std::vector<IRval>{ IRval::createVal(oper.args[0].getType(), 0ULL) };
        }
    }

    // Mul by power of 2
    if (oper.op == IR_MUL) {
        if (isConstEqual(oper.args[1], 2ULL)) {
            oper.op = IR_ADD;
            oper.args = std::vector<IRval>{ oper.args[0], oper.args[0] };
        }
        else if (isConstEqual(oper.args[0], 2ULL)) {
            oper.op = IR_ADD;
            oper.args = std::vector<IRval>{ oper.args[1], oper.args[1] };
        }
    }
}

std::shared_ptr<ControlFlowGraph> AlgebraicTransformer::getCfg() {
    return cfg;
}
