#include "constants_folder.hpp"
#include "utils.hpp"

std::optional<IRval> ConstantsFolder::foldExpr(const IR_Expr &expr) {
    switch (expr.type) {
        case IR_Expr::OPERATION:
            return foldOper(dynamic_cast<IR_ExprOper const &>(expr));
        case IR_Expr::CAST:
            return foldCast(dynamic_cast<IR_ExprCast const &>(expr));
        case IR_Expr::PHI:
            return foldPhi(dynamic_cast<IR_ExprPhi const &>(expr));

        case IR_Expr::CALL: // TODO: pure functions
        case IR_Expr::ALLOCATION:
        case IR_Expr::TERM:
            return {};

        default:
            internalError("Wrong IR expression type");
    }
}

std::optional<IRval> ConstantsFolder::foldOper(const IR_ExprOper &expr) {
    if (isInList(expr.op, { IR_ExprOper::LOAD, IR_ExprOper::STORE }))
        return {};

    // TODO: extract from aggregate?
    for (auto const &arg : expr.args)
        if (arg.getValueClass() != IRval::VAL)
            return {};

    if (isGeneralNumOp(expr.op)) {
        return std::visit([&expr](auto const &l) -> IRval {
            auto const &r = std::get<std::remove_cvref_t<decltype(l)>>(expr.args[1].getVal());
            switch (expr.op) {
                case IR_ExprOper::ADD:
                    return IRval::createVal(expr.args[0].getType(), l + r);
                case IR_ExprOper::SUB:
                    return IRval::createVal(expr.args[0].getType(), l - r);
                case IR_ExprOper::MUL:
                    return IRval::createVal(expr.args[0].getType(), l * r);
                case IR_ExprOper::DIV:
                    return IRval::createVal(expr.args[0].getType(), l / r);
                default:
                    internalError("Wrong general arithmetic operation");
            }
        }, expr.args[0].getVal());
    }
    else if (isComparsionOp(expr.op)) {
        return std::visit([&expr](auto const &l) -> IRval {
            auto const &r = std::get<std::remove_cvref_t<decltype(l)>>(expr.args[1].getVal());
            // TODO: i1
            switch (expr.op) {
                case IR_ExprOper::LAND:
                    return IRval::createVal(IR_TypeDirect::type_i8, static_cast<int8_t>(l && r));
                case IR_ExprOper::LOR:
                    return IRval::createVal(IR_TypeDirect::type_i8, static_cast<int8_t>(l || r));
                case IR_ExprOper::EQ:
                    return IRval::createVal(IR_TypeDirect::type_i8, static_cast<int8_t>(l == r));
                case IR_ExprOper::NE:
                    return IRval::createVal(IR_TypeDirect::type_i8, static_cast<int8_t>(l != r));
                case IR_ExprOper::GT:
                    return IRval::createVal(IR_TypeDirect::type_i8, static_cast<int8_t>(l > r));
                case IR_ExprOper::LT:
                    return IRval::createVal(IR_TypeDirect::type_i8, static_cast<int8_t>(l < r));
                case IR_ExprOper::GE:
                    return IRval::createVal(IR_TypeDirect::type_i8, static_cast<int8_t>(l >= r));
                case IR_ExprOper::LE:
                    return IRval::createVal(IR_TypeDirect::type_i8, static_cast<int8_t>(l <= r));
                default:
                    internalError("Wrong comparsion operation");
            }
        }, expr.args[0].getVal());
    }
    else if (isIntegerOp(expr.op)) {
        // TODO: make it prettier
        using intVariant_t = std::variant<uint8_t, int8_t, uint32_t, int32_t, uint64_t, int64_t>;
        intVariant_t intVariant = variant_cast(expr.args[0].getVal());
        return std::visit([&expr](auto const &l) -> IRval {
            auto const &r = std::get<std::remove_cvref_t<decltype(l)>>(expr.args[1].getVal());
            switch (expr.op) {
                case IR_ExprOper::REM:
                    return IRval::createVal(expr.args[0].getType(), l % r);
                case IR_ExprOper::SHR:
                    return IRval::createVal(expr.args[0].getType(), l >> r);
                case IR_ExprOper::SHL:
                    return IRval::createVal(expr.args[0].getType(), l << r);
                case IR_ExprOper::XOR:
                    return IRval::createVal(expr.args[0].getType(), l ^ r);
                case IR_ExprOper::AND:
                    return IRval::createVal(expr.args[0].getType(), l & r);
                case IR_ExprOper::OR:
                    return IRval::createVal(expr.args[0].getType(), l | r);
                default:
                    internalError("Wrong integer operation");
            }
        }, intVariant);
    }
    else {
        return {};
    }
}


constexpr bool ConstantsFolder::isGeneralNumOp(IR_ExprOper::IR_Ops op) {
    using o = IR_ExprOper;
    return isInList(op, { o::ADD, o::SUB, o::DIV, o::MUL });
}

constexpr bool ConstantsFolder::isIntegerOp(IR_ExprOper::IR_Ops op) {
    using o = IR_ExprOper;
    return isInList(op, { o::SHL, o::SHR, o::REM, o::AND, o::OR, o::XOR });
}

/** Includes short-logic operations */
constexpr bool ConstantsFolder::isComparsionOp(IR_ExprOper::IR_Ops op) {
    using o = IR_ExprOper;
    return isInList(op, { o::LT, o::GT, o::LE, o::GE, o::EQ, o::NE, o::LAND, o::LOR });
}

constexpr bool ConstantsFolder::isShortLogicOp(IR_ExprOper::IR_Ops op) {
    using o = IR_ExprOper;
    return isInList(op, { o::LAND, o::LOR });
}


std::optional<IRval> ConstantsFolder::foldCast(const IR_ExprCast &expr) {
    if (expr.dest->type != IR_Type::DIRECT)
        return {};
    if (expr.arg.getValueClass() != IRval::VAL)
        return {};

    auto dirType = std::dynamic_pointer_cast<IR_TypeDirect>(expr.dest);
    switch (dirType->spec) {
        case IR_TypeDirect::I8:
            return IRval::createVal(IR_TypeDirect::type_i8, expr.arg.castValTo<int8_t>());
        case IR_TypeDirect::U8:
            return IRval::createVal(IR_TypeDirect::type_u8, expr.arg.castValTo<uint8_t>());
        case IR_TypeDirect::I32:
            return IRval::createVal(IR_TypeDirect::type_i32, expr.arg.castValTo<int32_t>());
        case IR_TypeDirect::U32:
            return IRval::createVal(IR_TypeDirect::type_u32, expr.arg.castValTo<uint32_t>());
        case IR_TypeDirect::I64:
            return IRval::createVal(IR_TypeDirect::type_i64, expr.arg.castValTo<int64_t>());
        case IR_TypeDirect::U64:
            return IRval::createVal(IR_TypeDirect::type_u64, expr.arg.castValTo<uint64_t>());
        case IR_TypeDirect::F32:
            return IRval::createVal(IR_TypeDirect::type_f32, expr.arg.castValTo<float>());
        case IR_TypeDirect::F64:
            return IRval::createVal(IR_TypeDirect::type_f64, expr.arg.castValTo<double>());

        case IR_TypeDirect::VOID:
            internalError("Cast to void is prohibited");
        default:
            internalError("Wrong direct type");
    }
}

std::optional<IRval> ConstantsFolder::foldPhi(const IR_ExprPhi &expr) {
    if (expr.args.empty())
        internalError("Empty phi");

    IRval const &common = expr.args.at(0);
    for (auto const &[pos, arg] : expr.args)
        if (!arg.equal(common))
            return {};
    return common;
}
