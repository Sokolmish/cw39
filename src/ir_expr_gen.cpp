#include "ir_generator.hpp"

[[nodiscard]]
static IRval doConstBinOperation(AST_Binop::OpType op, IRval const &lhs, IRval const &rhs) {
    using bop = AST_Binop;

    if (!lhs.isConstant() || !rhs.isConstant())
        semanticError("Non constant sub expressions");
    if (lhs.getType()->type != IR_Type::DIRECT || rhs.getType()->type != IR_Type::DIRECT)
        semanticError("Pointers arithmetics is not constant");

    auto ltype = dynamic_cast<IR_TypeDirect const &>(*lhs.getType());
    auto rtype = dynamic_cast<IR_TypeDirect const &>(*rhs.getType());

    if (isInList(op, { bop::SHL, bop::SHR, bop::REM, bop::BIT_AND, bop::BIT_OR, bop::BIT_XOR,
                       bop::LOG_AND, bop::LOG_OR })) {
        if (ltype.isFloat() || rtype.isFloat())
            semanticError("Invalid operation on floats");

        auto val1 = lhs.castValTo<uint64_t>();
        auto val2 = rhs.castValTo<uint64_t>();
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
    else if (isInList(op, { bop::ADD, bop::SUB, bop::DIV, bop::REM })) {
        // TODO: check type in variant and stored type are equal
        auto resVal = std::visit([&rhs, op](auto const &l) -> IRval::union_type {
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
    else if (isInList(op, { bop::LT, bop::GT, bop::LE, bop::GE, bop::EQ, bop::NE })) {
        auto resVal = std::visit([&rhs, op](auto const &l) -> uint64_t {
            return std::visit([l, op](auto const &r) -> uint64_t {
                switch (op) {
                    case bop::LT:
                        return l < r;
                    case bop::GT:
                        return l > r;
                    case bop::LE:
                        return l <= r;
                    case bop::GE:
                        return l >= r;
                    case bop::EQ:
                        return l == r;
                    case bop::NE:
                        return l != r;
                    default:
                        semanticError("WTF");
                }
            }, rhs.getVal());
        }, lhs.getVal());

        return IRval::createVal(std::make_unique<IR_TypeDirect>(IR_TypeDirect::I32), resVal);
    }

    throw;
}

std::optional<IRval> evalConstantExpr(AST_Expr const &node) {
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

            auto dirType = dynamic_cast<IR_TypeDirect const &>(*destType);
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

            auto dirType = dynamic_cast<IR_TypeDirect const &>(*arg->getType());

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

            uint64_t val = std::get<uint64_t>(expr.v);
            return IRval::createVal(std::make_unique<IR_TypeDirect>(IR_TypeDirect::I32), val);
        }

        default: {
            semanticError("Unexpected node type in expression");
        }
    }
}



IRval IR_Generator::evalExpr(AST_Expr const &node) {
    switch (node.node_type) {
        case AST_COMMA_EXPR: {
            auto const &expr = dynamic_cast<AST_CommaExpression const &>(node);
            if (expr.children.empty())
                semanticError("Empty comma expression");
            for (size_t i = 0; i < expr.children.size() - 1; i++)
                evalExpr(*expr.children[i]);
            return evalExpr(*expr.children.back());
        }

        case AST_ASSIGNMENT: {
            NOT_IMPLEMENTED("assignment");
        }

        case AST_TERNARY: {
            NOT_IMPLEMENTED("ternary");
        }

        case AST_BINOP: {
            using bop = AST_Binop;

            auto const &expr = dynamic_cast<AST_Binop const &>(node);
            auto lhs = evalExpr(*expr.lhs);
            auto rhs = evalExpr(*expr.rhs);
            // TODO: check equal types

            if (expr.op == bop::ADD) {
                auto val = std::make_unique<IR_ExprOper>(
                        IR_ADD, std::vector<IRval>{ std::move(lhs), std::move(rhs) });
                auto res = IRval::createReg(lhs.getType());
                curBlock->addNode(IR_Node{ res, std::move(val) });
                return res;
            }
            else {
                NOT_IMPLEMENTED("");
            }
            break;
        }

        case AST_CAST: {
            NOT_IMPLEMENTED("cast");
        }

        case AST_UNARY_OP: {
//            using uop = AST_Unop;
//
            auto const &expr = dynamic_cast<AST_Unop const &>(node);
            auto arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));

//            if (expr.op == uop::DEREF) {
//                auto val = std::make_unique<IR_ExprOper>(
//                        IR_DEREF,
//                        std::vector<IRval>{ std::move(lhs), std::move(rhs) });
//                auto res = IRval::createReg(lhs.getType());
//                block.addNode(IR_Node{ res, std::move(val) });
//            }
            NOT_IMPLEMENTED("");
        }

        case AST_POSTFIX: {
//            auto const &expr = dynamic_cast<AST_Postfix const &>(node);
            NOT_IMPLEMENTED("");
        }

        case AST_PRIMARY: {
            auto const &expr = dynamic_cast<AST_Primary const &>(node);

            if (expr.type == AST_Primary::CONST) {
                uint64_t val = std::get<uint64_t>(expr.v);
                return IRval::createVal(std::make_unique<IR_TypeDirect>(IR_TypeDirect::I32), val);
            }
            else if (expr.type == AST_Primary::IDENT) {
                auto var = variables.get(std::get<string_id_t>(expr.v));
                if (!var.has_value())
                    semanticError("Unknown variable");

                auto val = std::make_unique<IR_ExprOper>(
                        IR_DEREF, std::vector<IRval>{ *var });
                auto const &ptrType = dynamic_cast<IR_TypePtr const &>(*var->getType());
                auto res = IRval::createReg(ptrType.child);
                curBlock->addNode(IR_Node{ res, std::move(val) });
                return res;
            }

            NOT_IMPLEMENTED("");
        }

        default: {
            semanticError("Unexpected node type in expression");
        }
    }
    throw;
}
