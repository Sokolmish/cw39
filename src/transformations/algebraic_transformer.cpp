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

 /**
 * Returns log2(v) if it is whole or -1 otherwise
 */
static int getTwosPower(uint64_t v) {
     if (!v)
         return 0;
     if ((v & (v - 1)) != 0)
         return -1;

     int res = 0;
     while (v >>= 1)
         res++;
     return res;
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
        // TODO: see note at comparsion operator
        if (oper.args[0] == oper.args[1] && oper.args[0].version == oper.args[1].version) {
            oper.op = IR_ExprOper::MOV;
            oper.args = std::vector<IRval>{ IRval::createVal(oper.args[0].getType(), 0ULL) };
        }
    }

    // Mul by power of 2
    if (oper.op == IR_ExprOper::MUL) {
        if (oper.args[0].isConstant() && oper.args[0].getType()->type == IR_Type::DIRECT) {
            auto dirType = std::dynamic_pointer_cast<IR_TypeDirect>(oper.args[0].getType());
            if (dirType->isInteger()) {
                int log = getTwosPower(oper.args[0].castValTo<uint64_t>());
                if (log >= 0) {
                    oper.op = IR_ExprOper::SHL;
                    oper.args = std::vector<IRval>{
                            oper.args[1],
                            IRval::createVal(oper.args[0].getType(), static_cast<uint64_t>(log)) };
                }
            }
        }
        if (oper.args[1].isConstant() && oper.args[1].getType()->type == IR_Type::DIRECT) {
            auto dirType = std::dynamic_pointer_cast<IR_TypeDirect>(oper.args[1].getType());
            if (dirType->isInteger()) {
                int log = getTwosPower(oper.args[1].castValTo<uint64_t>());
                if (log >= 0) {
                    oper.op = IR_ExprOper::SHL;
                    oper.args = std::vector<IRval>{
                            oper.args[0],
                            IRval::createVal(oper.args[0].getType(), static_cast<uint64_t>(log)) };
                }
            }
        }
    }

    // Div by power of 2
    if (oper.op == IR_ExprOper::DIV) {
        auto dirType = std::dynamic_pointer_cast<IR_TypeDirect>(oper.args[1].getType());
        if (dirType->isInteger()) {
            if (oper.args[1].isConstant() && oper.args[1].getType()->type == IR_Type::DIRECT) {
                int log = getTwosPower(oper.args[1].castValTo<uint64_t>());
                if (log >= 0) {
                    oper.op = IR_ExprOper::SHR;
                    oper.args = std::vector<IRval>{
                            oper.args[0],
                            IRval::createVal(oper.args[1].getType(), static_cast<uint64_t>(log)) };
                }
            }
        }
    }
}

std::shared_ptr<ControlFlowGraph> AlgebraicTransformer::getCfg() {
    return cfg;
}
