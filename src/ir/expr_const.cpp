#include "generator.hpp"

// TODO: is shortLogic

bool IR_Generator::isGeneralNumOp(AST_Binop::OpType op) {
    using bop = AST_Binop;
    return isInList(op, { bop::ADD, bop::SUB, bop::DIV, bop::MUL });
}

bool IR_Generator::isIntegerNumOp(AST_Binop::OpType op) {
    using bop = AST_Binop;
    return isInList(op, { bop::SHL, bop::SHR, bop::REM,
                          bop::BIT_AND, bop::BIT_OR, bop::BIT_XOR,
                          bop::LOG_AND, bop::LOG_OR });
}

bool IR_Generator::isComparsionOp(AST_Binop::OpType op) {
    using bop = AST_Binop;
    return isInList(op, { bop::LT, bop::GT, bop::LE, bop::GE, bop::EQ, bop::NE });
}


IRval IR_Generator::doConstBinOperation(AST_Binop::OpType op, IRval const &lhs, IRval const &rhs) {
    using bop = AST_Binop;

    if (!lhs.isConstant() || !rhs.isConstant())
        semanticError("Non constant sub expressions");
    if (lhs.getType()->type != IR_Type::DIRECT || rhs.getType()->type != IR_Type::DIRECT)
        semanticError("Pointers arithmetics is not constant");

    auto const &ltype = dynamic_cast<IR_TypeDirect const &>(*lhs.getType());
    auto const &rtype = dynamic_cast<IR_TypeDirect const &>(*rhs.getType());

    if (!lhs.getType()->equal(*rhs.getType()))
        semanticError("Cannot do binary operation on different types");

    if (isIntegerNumOp(op)) {
        if (ltype.isFloat() || rtype.isFloat())
            semanticError("Invalid operation on floats");

        uint64_t val1 = lhs.castValTo<uint64_t>(); // TODO: signedness
        uint64_t val2 = rhs.castValTo<uint64_t>();
        IRval::union_type resVal;
        switch (op) {
            case bop::SHL:
                resVal = (val1 << val2);
                break;
            case bop::SHR:
                resVal = (val1 >> val2);
                break;
            case bop::REM:
                resVal = (val1 % val2);
                break;
            case bop::BIT_XOR:
                resVal = (val1 ^ val2);
                break;
            case bop::BIT_AND:
                resVal = (val1 & val2);
                break;
            case bop::BIT_OR:
                resVal = (val1 | val2);
                break;
            case bop::LOG_OR:
                resVal = static_cast<uint64_t>(val1 || val2);
                break;
            case bop::LOG_AND:
                resVal = static_cast<uint64_t>(val1 && val2);
                break;
            default:
                semanticError("WTF");
        }

        int maxSize = std::max(ltype.getBytesSize(), rtype.getBytesSize());
        bool isUnsigned = ltype.isUnsigned() || rtype.isUnsigned();
        IR_TypeDirect::DirType dirType;
        // TODO: maybe truncate values
        if (maxSize == 1)
            dirType = isUnsigned ? IR_TypeDirect::U8 : IR_TypeDirect::I8;
        else if (maxSize == 4)
            dirType = isUnsigned ? IR_TypeDirect::U32 : IR_TypeDirect::I32;
        else if (maxSize == 8)
            dirType = isUnsigned ? IR_TypeDirect::U64 : IR_TypeDirect::I64;
        else
            semanticError("WTF");
        return IRval::createVal(std::make_unique<IR_TypeDirect>(dirType), resVal);
    }
    else if (isGeneralNumOp(op)) {
        // TODO: check type in variant and stored type are equal
        auto const &resVal = std::visit([&rhs, op](auto const &l) -> IRval::union_type {
            return std::visit([l, op](auto const &r) -> IRval::union_type {
                switch (op) {
                    case bop::ADD:
                        return l + r;
                    case bop::SUB:
                        return l - r;
                    case bop::MUL:
                        return l * r;
                    case bop::DIV:
                        return l / r;
                    default:
                        semanticError("WTF");
                }
            }, rhs.getVal());
        }, lhs.getVal());

        int maxSize = std::max(ltype.getBytesSize(), rtype.getBytesSize());
        bool isUnsigned = ltype.isUnsigned() || rtype.isUnsigned();
        IR_TypeDirect::DirType dirType;
        // TODO: maybe truncate values
        if (ltype.isFloat() || rtype.isFloat()) {
            if (ltype.spec == IR_TypeDirect::U64 || rtype.spec == IR_TypeDirect::U64)
                dirType = IR_TypeDirect::U64;
            else
                dirType = IR_TypeDirect::U32;
        } else if (maxSize == 1) {
            dirType = isUnsigned ? IR_TypeDirect::U8 : IR_TypeDirect::I8;
        } else if (maxSize == 4) {
            dirType = isUnsigned ? IR_TypeDirect::U32 : IR_TypeDirect::I32;
        } else if (maxSize == 8) {
            dirType = isUnsigned ? IR_TypeDirect::U64 : IR_TypeDirect::I64;
        } else {
            semanticError("WTF");
        }

        return IRval::createVal(std::make_unique<IR_TypeDirect>(dirType), resVal);
    }
    else if (isComparsionOp(op)) {
        auto resVal = std::visit([&rhs, op](auto const &l) -> uint64_t {
            return std::visit([l, op](auto const &r) -> uint64_t {
                switch (op) {
                    case bop::LT:
                        return l < static_cast<decltype(l)>(r);
                    case bop::GT:
                        return l > static_cast<decltype(l)>(r);
                    case bop::LE:
                        return l <= static_cast<decltype(l)>(r);
                    case bop::GE:
                        return l >= static_cast<decltype(l)>(r);
                    case bop::EQ:
                        return l == static_cast<decltype(l)>(r);
                    case bop::NE:
                        return l != static_cast<decltype(l)>(r);
                    default:
                        semanticError("WTF");
                }
            }, rhs.getVal());
        }, lhs.getVal());

        return IRval::createVal(IR_TypeDirect::type_i32, resVal);
    }

    throw;
}

std::optional<IRval> IR_Generator::evalConstantExpr(AST_Expr const &node) {
    switch (node.node_type) {
        case AST_COMMA_EXPR: {
            return evalConstantExpr(*dynamic_cast<AST_CommaExpression const &>(node).children.back());
        }

        case AST_ASSIGNMENT: {
            semanticError("Assignment is not constant expression");
        }

        case AST_TERNARY: {
            auto const &expr = dynamic_cast<AST_Ternary const &>(node);
            auto cond = evalConstantExpr(*expr.cond);
            auto v_tr = evalConstantExpr(*expr.v_true);
            auto v_fl = evalConstantExpr(*expr.v_false);
            if (!(cond.has_value() && v_tr.has_value() && v_fl.has_value()))
                semanticError("Expression is not constant");
            return std::move(cond->castValTo<uint64_t>() ? v_tr : v_fl);
        }

        case AST_BINOP: {
            auto const &expr = dynamic_cast<AST_Binop const &>(node);
            auto lhs = evalConstantExpr(*expr.lhs);
            auto rhs = evalConstantExpr(*expr.rhs);
            if (!(lhs.has_value() && rhs.has_value()))
                semanticError("Expression is not constant");
            return doConstBinOperation(expr.op, *lhs, *rhs);
        }

        case AST_CAST: {
            auto const &expr = dynamic_cast<AST_Cast const &>(node);
            auto arg = evalConstantExpr(*expr.child);
            if (!arg->isConstant())
                semanticError("Expression is not constant");

            auto destType = getType(*expr.type_name);
            if (destType->type != IR_Type::DIRECT)
                NOT_IMPLEMENTED("");

            auto const &dirType = dynamic_cast<IR_TypeDirect const &>(*destType);
            IRval::union_type newVal;
            if (dirType.spec == IR_TypeDirect::F32)
                newVal = arg->castValTo<float>();
            else
                newVal = arg->castValTo<uint64_t>();

            return IRval::createVal(std::move(destType), newVal);
        }

        case AST_UNARY_OP: {
            using uop = AST_Unop;

            auto const &expr = dynamic_cast<AST_Unop const &>(node);
            auto arg = evalConstantExpr(dynamic_cast<AST_Expr const &>(*expr.child));
            if (!arg.has_value())
                semanticError("Expression is not constant");
            if (arg->getType()->type != IR_Type::DIRECT)
                semanticError("Not");
            if (expr.op == uop::SIZEOF_OP)
                NOT_IMPLEMENTED("sizeof");
            if (isInList(expr.op, { uop::PRE_INC, uop::PRE_DEC, uop::DEREF, uop::ADDR_OF }))
                semanticError("Operation is not constant");

            auto const &dirType = dynamic_cast<IR_TypeDirect const &>(*arg->getType());

            if (expr.op == uop::UN_PLUS) {
                return arg;
            }
            else if (expr.op == uop::UN_MINUS) {
                auto resVal = std::visit([](const auto &e) -> IRval::union_type {
                    return -e;
                }, arg->getVal());
                return IRval::createVal(std::make_unique<IR_TypeDirect>(dirType.spec), resVal);
            }
            else if (expr.op == uop::UN_NEG || expr.op == uop::UN_NOT) {
                if (dirType.isFloat())
                    semanticError("Wrong operation for float");
                IRval::union_type resVal = ~arg->castValTo<uint64_t>();
                return IRval::createVal(std::make_unique<IR_TypeDirect>(dirType.spec), resVal);
            }
            else {
                semanticError("WTF");
            }
        }

        case AST_POSTFIX: {
            NOT_IMPLEMENTED("");
        }

        case AST_PRIMARY: {
            auto const &expr = dynamic_cast<AST_Primary const &>(node);

            if (expr.type != AST_Primary::CONST)
                semanticError("Non constant expression");

            AST_Literal val = expr.getLiteral();
            if (val.type != INTEGER_LITERAL || val.longCnt != 0 || val.isUnsigned) {
                NOT_IMPLEMENTED("Non i32 literal");
            }
            return IRval::createVal(getLiteralType(val), static_cast<uint64_t>(val.val.vi32));
        }

        default: {
            semanticError("Unexpected node type in expression");
        }
    }
}
