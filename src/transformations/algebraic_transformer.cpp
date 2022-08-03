#include "computing_transformers.hpp"
#include "utils.hpp"
#include <bit>

AlgebraicTransformer::AlgebraicTransformer(IntermediateUnit const &unit, CFGraph rawCfg)
        : IRTransformer(std::move(rawCfg)) {
    std::set<int> visited;
    cfg.traverseBlocks(cfg.entryBlockId, visited, [this](int blockId) {
        auto &curBlock = cfg.block(blockId);
        for (auto *node : curBlock.getAllNodes()) {
            processNode(node);
        }
    });
}

static bool isConstEqual(IRval const &ir_val, uint64_t num_val) {
    if (!ir_val.isConstant())
        return false;
    auto dirType = ir_val.getType()->castType<IR_TypeDirect>();
    if (!dirType || !dirType->isInteger())
        return false;
    else
        return ir_val.castValTo<uint64_t>() == num_val;
}

void AlgebraicTransformer::processNode(IR_Node *node) {
    auto oper = node->body->toOper();
    if (!oper)
        return;

    // Add or subtract 0
    if (isInList(oper->op, IR_ExprOper::ADD, IR_ExprOper::SUB)) {
        if (isConstEqual(oper->args[0], 0ULL)) {
            if (oper->op == IR_ExprOper::ADD) {
                oper->op = IR_ExprOper::MOV;
                oper->args = std::vector<IRval>{ oper->args[1] };
                setPassChanged();
            }
        }
        else if (isConstEqual(oper->args[1], 0ULL)) {
            oper->op = IR_ExprOper::MOV;
            oper->args = std::vector<IRval>{ oper->args[0] };
            setPassChanged();
        }
    }

    // Mul by 0
    if (oper->op == IR_ExprOper::MUL) {
        if (isConstEqual(oper->args[0], 0ULL)) {
            oper->op = IR_ExprOper::MOV;
            oper->args = std::vector<IRval>{ oper->args[0] };
            setPassChanged();
        }
        else if (isConstEqual(oper->args[1], 0ULL)) {
            oper->op = IR_ExprOper::MOV;
            oper->args = std::vector<IRval>{ oper->args[1] };
            setPassChanged();
        }
    }

    // Division of 0
    if (isInList(oper->op, IR_ExprOper::DIV, IR_ExprOper::REM)) {
        if (isConstEqual(oper->args[0], 0ULL)) {
            oper->op = IR_ExprOper::MOV;
            oper->args = std::vector<IRval>{ oper->args[0] };
            setPassChanged();
        }
        else if (isConstEqual(oper->args[1], 0ULL)) {
            throw cw39_error("Division by zero");
        }
    }

    // Bit shift of 0 and by 0
    if (isInList(oper->op, IR_ExprOper::SHR, IR_ExprOper::SHL)) {
        if (isConstEqual(oper->args[1], 0ULL)) {
            oper->op = IR_ExprOper::MOV;
            oper->args = std::vector<IRval>{ oper->args[0] };
            setPassChanged();
        }
        else if (isConstEqual(oper->args[0], 0ULL)) {
            oper->op = IR_ExprOper::MOV;
            oper->args = std::vector<IRval>{ oper->args[0] };
            setPassChanged();
        }
    }

    // Mul by 1
    if (oper->op == IR_ExprOper::MUL) {
        if (isConstEqual(oper->args[1], 1ULL)) {
            oper->op = IR_ExprOper::MOV;
            oper->args = std::vector<IRval>{ oper->args[0] };
            setPassChanged();
        }
        else if (isConstEqual(oper->args[0], 1ULL)) {
            oper->op = IR_ExprOper::MOV;
            oper->args = std::vector<IRval>{ oper->args[1] };
            setPassChanged();
        }
    }

    // Div by 1
    if (oper->op == IR_ExprOper::DIV) {
        if (isConstEqual(oper->args[1], 1ULL)) {
            oper->op = IR_ExprOper::MOV;
            oper->args = std::vector<IRval>{ oper->args[0] };
            setPassChanged();
        }
    }

    // Subtraction or XOR on equal values
    if (isInList(oper->op, IR_ExprOper::SUB, IR_ExprOper::XOR)) {
        if (oper->args[0] == oper->args[1]) {
            oper->op = IR_ExprOper::MOV;
            oper->args = std::vector<IRval>{ IRval::createVal(oper->args[0].getType(), 0ULL) };
            setPassChanged();
        }
    }

    // Mul by power of 2
    if (oper->op == IR_ExprOper::MUL) {
        if (oper->args[0].isConstant()) {
            auto dirType = oper->args[0].getType()->castType<IR_TypeDirect>();
            if (dirType && dirType->isInteger()) {
                uint64_t val = oper->args[0].castValTo<uint64_t>();
                if (std::has_single_bit(val)) {
                    uint64_t log = static_cast<uint64_t>(std::bit_width(val) - 1);
                    oper->op = IR_ExprOper::SHL;
                    oper->args = std::vector<IRval>{
                        oper->args[1], IRval::createVal(oper->args[0].getType(), log) };
                    setPassChanged();
                }
            }
        }
        if (oper->args[1].isConstant()) {
            auto dirType = oper->args[1].getType()->castType<IR_TypeDirect>();
            if (dirType && dirType->isInteger()) {
                uint64_t val = oper->args[1].castValTo<uint64_t>();
                if (std::has_single_bit(val)) {
                    uint64_t log = static_cast<uint64_t>(std::bit_width(val) - 1);
                    oper->op = IR_ExprOper::SHL;
                    oper->args = std::vector<IRval>{
                        oper->args[0], IRval::createVal(oper->args[1].getType(), log) };
                    setPassChanged();
                }
            }
        }
    }

    // Div by power of 2
    if (oper->op == IR_ExprOper::DIV) {
        auto dirType = std::dynamic_pointer_cast<IR_TypeDirect>(oper->args[1].getType());
        if (dirType && dirType->isInteger()) {
            if (oper->args[1].isConstant()) {
                uint64_t val = oper->args[1].castValTo<uint64_t>();
                if (std::has_single_bit(val)) {
                    uint64_t log = static_cast<uint64_t>(std::bit_width(val) - 1);
                    oper->op = IR_ExprOper::SHR;
                    oper->args = std::vector<IRval>{
                        oper->args[0], IRval::createVal(std::move(dirType), log) };
                    setPassChanged();
                }
            }
        }
    }

    // Rem by power of 2
    if (oper->op == IR_ExprOper::REM) {
        auto dirType = std::dynamic_pointer_cast<IR_TypeDirect>(oper->args[1].getType());
        if (dirType && dirType->isUnsigned()) { // Signed REM is a bit more complex
            if (oper->args[1].isConstant()) {
                uint64_t val = oper->args[1].castValTo<uint64_t>();
                if (std::has_single_bit(val)) {
                    uint64_t log = static_cast<uint64_t>(std::bit_width(val) - 1);
                    uint64_t mask = (1 << log) - 1;
                    oper->op = IR_ExprOper::AND;
                    oper->args = std::vector<IRval>{
                        oper->args[0], IRval::createVal(std::move(dirType), mask) };
                    setPassChanged();
                }
            }
        }
    }
}
