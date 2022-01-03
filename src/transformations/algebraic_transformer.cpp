#include "algebraic_transformer.hpp"
#include "utils.hpp"
#include <bit>

AlgebraicTransformer::AlgebraicTransformer(std::shared_ptr<ControlFlowGraph> const &rawCfg)
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
    if (isInList(oper.op, { IR_ExprOper::ADD, IR_ExprOper::SUB })) {
        if (isConstEqual(oper.args[0], 0ULL)) {
            if (oper.op == IR_ExprOper::ADD) {
                oper.op = IR_ExprOper::MOV;
                oper.args = std::vector<IRval>{ oper.args[1] };
            }
        }
        else if (isConstEqual(oper.args[1], 0ULL)) {
            oper.op = IR_ExprOper::MOV;
            oper.args = std::vector<IRval>{ oper.args[0] };
        }
    }

    // Mul by 0
    if (isInList(oper.op, { IR_ExprOper::MUL })) {
        if (isConstEqual(oper.args[0], 0ULL)) {
            oper.op = IR_ExprOper::MOV;
            oper.args = std::vector<IRval>{ oper.args[0] };
        }
        else if (isConstEqual(oper.args[1], 0ULL)) {
            oper.op = IR_ExprOper::MOV;
            oper.args = std::vector<IRval>{ oper.args[1] };
        }
    }

    // Zero division
    if (isInList(oper.op, { IR_ExprOper::DIV, IR_ExprOper::REM })) {
        if (isConstEqual(oper.args[0], 0ULL)) {
            oper.op = IR_ExprOper::MOV;
            oper.args = std::vector<IRval>{ oper.args[0] };
        }
        else if (isConstEqual(oper.args[1], 0ULL)) {
            semanticError("Division by zero");
        }
    }

    // Bit shift of 0 and by 0
    if (isInList(oper.op, { IR_ExprOper::SHR, IR_ExprOper::SHL })) {
        if (isConstEqual(oper.args[1], 0ULL)) {
            oper.op = IR_ExprOper::MOV;
            oper.args = std::vector<IRval>{ oper.args[0] };
        }
        else if (isConstEqual(oper.args[0], 0ULL)) {
            oper.op = IR_ExprOper::MOV;
            oper.args = std::vector<IRval>{ oper.args[0] };
        }
    }

    // Mul by 1
    if (isInList(oper.op, { IR_ExprOper::MUL })) {
        if (isConstEqual(oper.args[1], 1ULL)) {
            oper.op = IR_ExprOper::MOV;
            oper.args = std::vector<IRval>{ oper.args[0] };
        }
        else if (isConstEqual(oper.args[0], 1ULL)) {
            oper.op = IR_ExprOper::MOV;
            oper.args = std::vector<IRval>{ oper.args[1] };
        }
    }

    // Div by 1
    if (isInList(oper.op, { IR_ExprOper::MUL })) {
        if (isConstEqual(oper.args[1], 1ULL)) {
            oper.op = IR_ExprOper::MOV;
            oper.args = std::vector<IRval>{ oper.args[0] };
        }
    }

    // Zero subtraction
    if (isInList(oper.op, { IR_ExprOper::SUB, IR_ExprOper::XOR })) {
        if (oper.args[0].equal(oper.args[1])) {
            oper.op = IR_ExprOper::MOV;
            oper.args = std::vector<IRval>{ IRval::createVal(oper.args[0].getType(), 0ULL) };
        }
    }

    // Mul by power of 2
    if (oper.op == IR_ExprOper::MUL) {
        if (oper.args[0].isConstant() && oper.args[0].getType()->type == IR_Type::DIRECT) {
            auto dirType = std::dynamic_pointer_cast<IR_TypeDirect>(oper.args[0].getType());
            if (dirType->isInteger()) {
                uint64_t val = oper.args[0].castValTo<uint64_t>();
                if (std::has_single_bit(val)) {
                    uint64_t log = static_cast<uint64_t>(std::bit_width(val) - 1);
                    oper.op = IR_ExprOper::SHL;
                    oper.args = std::vector<IRval>{ oper.args[1], IRval::createVal(dirType, log) };
                }
            }
        }
        if (oper.args[1].isConstant() && oper.args[1].getType()->type == IR_Type::DIRECT) {
            auto dirType = std::dynamic_pointer_cast<IR_TypeDirect>(oper.args[1].getType());
            if (dirType->isInteger()) {
                uint64_t val = oper.args[1].castValTo<uint64_t>();
                if (std::has_single_bit(val)) {
                    uint64_t log = static_cast<uint64_t>(std::bit_width(val) - 1);
                    oper.op = IR_ExprOper::SHL;
                    oper.args = std::vector<IRval>{ oper.args[0], IRval::createVal(dirType, log) };
                }
            }
        }
    }

    // Div by power of 2
    if (oper.op == IR_ExprOper::DIV) {
        auto dirType = std::dynamic_pointer_cast<IR_TypeDirect>(oper.args[1].getType());
        if (dirType->isInteger()) {
            if (oper.args[1].isConstant() && oper.args[1].getType()->type == IR_Type::DIRECT) {
                uint64_t val = oper.args[1].castValTo<uint64_t>();
                if (std::has_single_bit(val)) {
                    uint64_t log = static_cast<uint64_t>(std::bit_width(val) - 1);
                    oper.op = IR_ExprOper::SHR;
                    oper.args = std::vector<IRval>{ oper.args[0], IRval::createVal(dirType, log) };
                }
            }
        }
    }
}

std::shared_ptr<ControlFlowGraph> AlgebraicTransformer::getCfg() {
    return cfg;
}
