#include "constants_folder.hpp"
#include "utils.hpp"
#include <bit>

std::optional<IRval> ConstantsFolder::foldExpr(IR_Expr const &expr) {
    if (auto exprOper = expr.toOper())
        return foldOper(*exprOper);
    else if (auto exprCast = expr.toCast())
        return foldCast(*exprCast);
    else if (auto exprPhi = expr.toPHI())
        return foldPhi(*exprPhi);
    else
        return std::nullopt;
}

std::optional<IRval> ConstantsFolder::foldOper(const IR_ExprOper &expr) {
    // TODO: extract from aggregate?
    for (auto const &arg : expr.args)
        if (arg.getValueClass() != IRval::VAL)
            return std::nullopt;

    if (isGeneralNumOp(expr.op)) {
        return std::visit([&expr](auto const &l) -> IRval {
            // TODO: create constants with exact variant type
//            auto const &r = std::get<std::remove_cvref_t<decltype(l)>>(expr.args[1].getVal());
            auto const &r = expr.args[1].castValTo<std::remove_cvref_t<decltype(l)>>();
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
                    throw cw39_internal_error("Wrong general arithmetic operation");
            }
        }, expr.args[0].getVal());
    }
    else if (isComparsionOp(expr.op)) {
        return std::visit([&expr](auto const &l) -> IRval {
//            auto const &r = std::get<std::remove_cvref_t<decltype(l)>>(expr.args[1].getVal());
            auto const &r = expr.args[1].castValTo<std::remove_cvref_t<decltype(l)>>();
            switch (expr.op) {
                case IR_ExprOper::LAND:
                    return IRval::createVal(IR_TypeDirect::getI1(), static_cast<int8_t>(l && r));
                case IR_ExprOper::LOR:
                    return IRval::createVal(IR_TypeDirect::getI1(), static_cast<int8_t>(l || r));
                case IR_ExprOper::EQ:
                    return IRval::createVal(IR_TypeDirect::getI1(), static_cast<int8_t>(l == r));
                case IR_ExprOper::NE:
                    return IRval::createVal(IR_TypeDirect::getI1(), static_cast<int8_t>(l != r));
                case IR_ExprOper::GT:
                    return IRval::createVal(IR_TypeDirect::getI1(), static_cast<int8_t>(l > r));
                case IR_ExprOper::LT:
                    return IRval::createVal(IR_TypeDirect::getI1(), static_cast<int8_t>(l < r));
                case IR_ExprOper::GE:
                    return IRval::createVal(IR_TypeDirect::getI1(), static_cast<int8_t>(l >= r));
                case IR_ExprOper::LE:
                    return IRval::createVal(IR_TypeDirect::getI1(), static_cast<int8_t>(l <= r));
                default:
                    throw cw39_internal_error("Wrong comparsion operation");
            }
        }, expr.args[0].getVal());
    }
    else if (isIntegerOp(expr.op)) {
        using intVariant_t = std::variant<uint8_t, int8_t, uint32_t, int32_t, uint64_t, int64_t>;
        intVariant_t intVariant = variant_cast(expr.args[0].getVal());
        return std::visit([&expr](auto const &l) -> IRval {
            auto const &r = expr.args[1].castValTo<std::remove_cvref_t<decltype(l)>>();
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
                    throw cw39_internal_error("Wrong integer operation");
            }
        }, intVariant);
    }
    else if (isIntrinsic(expr.op) && expr.op != IR_ExprOper::INTR_BITREV) {
        // No bitrev because it isn't in standard library
        using uintVariant_t = std::variant<uint8_t, uint32_t, uint64_t>;
        uintVariant_t intVariant = variant_cast(expr.args[0].getVal());
        return std::visit([&expr](auto const &a) -> IRval {
            switch (expr.op) {
                case IR_ExprOper::INTR_CTZ:
                    return IRval::createVal(expr.args[0].getType(), std::countr_zero(a));
                case IR_ExprOper::INTR_CLZ:
                    return IRval::createVal(expr.args[0].getType(), std::countl_zero(a));
                case IR_ExprOper::INTR_POPCNT:
                    return IRval::createVal(expr.args[0].getType(), std::popcount(a));
                default:
                    throw cw39_internal_error("Wrong intrinsic operation");
            }
        }, intVariant);
    }
    else {
        return std::nullopt;
    }
}


constexpr bool ConstantsFolder::isGeneralNumOp(IR_ExprOper::IR_Ops op) {
    using o = IR_ExprOper;
    return isInList(op, o::ADD, o::SUB, o::DIV, o::MUL);
}

constexpr bool ConstantsFolder::isIntegerOp(IR_ExprOper::IR_Ops op) {
    using o = IR_ExprOper;
    return isInList(op, o::SHL, o::SHR, o::REM, o::AND, o::OR, o::XOR);
}

/** Includes short-logic operations */
constexpr bool ConstantsFolder::isComparsionOp(IR_ExprOper::IR_Ops op) {
    using o = IR_ExprOper;
    return isInList(op, o::LT, o::GT, o::LE, o::GE, o::EQ, o::NE, o::LAND, o::LOR);
}

constexpr bool ConstantsFolder::isShortLogicOp(IR_ExprOper::IR_Ops op) {
    using o = IR_ExprOper;
    return isInList(op, o::LAND, o::LOR);
}

constexpr bool ConstantsFolder::isIntrinsic(IR_ExprOper::IR_Ops op) {
    using o = IR_ExprOper;
    return isInList(op, o::INTR_CTZ, o::INTR_CLZ, o::INTR_POPCNT, o::INTR_BITREV);
}


std::optional<IRval> ConstantsFolder::foldCast(const IR_ExprCast &expr) {
    if (expr.dest->type != IR_Type::DIRECT)
        return std::nullopt;
    if (expr.arg.getValueClass() != IRval::VAL)
        return std::nullopt;

    auto dirType = std::dynamic_pointer_cast<IR_TypeDirect>(expr.dest);
    switch (dirType->spec) {
        case IR_TypeDirect::I8:
            return IRval::createVal(IR_TypeDirect::getI8(), expr.arg.castValTo<int8_t>());
        case IR_TypeDirect::U8:
            return IRval::createVal(IR_TypeDirect::getU8(), expr.arg.castValTo<uint8_t>());
        case IR_TypeDirect::I32:
            return IRval::createVal(IR_TypeDirect::getI32(), expr.arg.castValTo<int32_t>());
        case IR_TypeDirect::U32:
            return IRval::createVal(IR_TypeDirect::getU32(), expr.arg.castValTo<uint32_t>());
        case IR_TypeDirect::I64:
            return IRval::createVal(IR_TypeDirect::getI64(), expr.arg.castValTo<int64_t>());
        case IR_TypeDirect::U64:
            return IRval::createVal(IR_TypeDirect::getU64(), expr.arg.castValTo<uint64_t>());
        case IR_TypeDirect::F32:
            return IRval::createVal(IR_TypeDirect::getF32(), expr.arg.castValTo<float>());
        case IR_TypeDirect::F64:
            return IRval::createVal(IR_TypeDirect::getF64(), expr.arg.castValTo<double>());

        case IR_TypeDirect::VOID:
            throw cw39_internal_error("Cast to void is prohibited"); // ???
        default:
            throw std::logic_error("Wrong direct type");
    }
}

std::optional<IRval> ConstantsFolder::foldPhi(const IR_ExprPhi &expr) {
    if (expr.args.empty())
        throw cw39_internal_error("Empty phi function");

    IRval const &common = expr.args.at(0);
    for (auto const &[pos, arg] : expr.args) {
        if (arg != common)
            return std::nullopt;
    }
    return common;
}
