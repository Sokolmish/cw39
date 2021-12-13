#include "generator.hpp"

[[nodiscard]]
static bool isGeneralNumOp(AST_Binop::OpType op) {
    using bop = AST_Binop;
    return isInList(op, { bop::ADD, bop::SUB, bop::DIV, bop::MUL });
}

[[nodiscard]]
static bool isIntegerNumOp(AST_Binop::OpType op) {
    using bop = AST_Binop;
    return isInList(op, { bop::SHL, bop::SHR, bop::REM,
                          bop::BIT_AND, bop::BIT_OR, bop::BIT_XOR,
                          bop::LOG_AND, bop::LOG_OR });
}

[[nodiscard]]
static bool isComparsionOp(AST_Binop::OpType op) {
    using bop = AST_Binop;
    return isInList(op, { bop::LT, bop::GT, bop::LE, bop::GE, bop::EQ, bop::NE });
}


[[nodiscard]]
static IRval doConstBinOperation(AST_Binop::OpType op, IRval const &lhs, IRval const &rhs) {
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

            AST_Literal val = std::get<AST_Literal>(expr.v);
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
            auto const &expr = dynamic_cast<AST_Assignment const &>(node);
            if (expr.lhs->node_type != AST_PRIMARY)
                semanticError("Only variables can be assigned");
            auto const &var = dynamic_cast<AST_Primary const &>(*expr.lhs);
            if (var.type != AST_Primary::IDENT)
                semanticError("Only variables can be assigned");

            // TODO: check types

            auto destVar = variables.get(std::get<string_id_t>(var.v));
            if (!destVar.has_value())
                semanticError("Unknown variable");
            auto rhsVal = evalExpr(*expr.rhs);
            auto val = std::make_unique<IR_ExprOper>(
                    IR_STORE, std::vector<IRval>{ *destVar, rhsVal });
            curBlock().addNode(IR_Node(std::move(val)));
            return rhsVal;
        }

        case AST_TERNARY: {
            NOT_IMPLEMENTED("ternary");
        }

        case AST_BINOP: {
            using bop = AST_Binop;

            auto const &expr = dynamic_cast<AST_Binop const &>(node);
            auto lhs = evalExpr(*expr.lhs);
            auto rhs = evalExpr(*expr.rhs);

            if (!lhs.getType()->equal(*rhs.getType()))
                semanticError("Cannot do binary operation on different types");

            // TODO: pointers arithmetics
            if (lhs.getType()->type != IR_Type::DIRECT)
                NOT_IMPLEMENTED("Pointers arithmetics");

            auto const &ltype = dynamic_cast<IR_TypeDirect const &>(*lhs.getType());
//            auto const &rtype = dynamic_cast<IR_TypeDirect const &>(*rhs.getType());

            if (isGeneralNumOp(expr.op)) {
                std::unique_ptr<IR_ExprOper> val;

                if (expr.op == bop::ADD)
                    val = std::make_unique<IR_ExprOper>(IR_ADD, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::SUB)
                    val = std::make_unique<IR_ExprOper>(IR_SUB, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::MUL)
                    val = std::make_unique<IR_ExprOper>(IR_MUL, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::DIV)
                    val = std::make_unique<IR_ExprOper>(IR_DIV, std::vector<IRval>{ lhs, rhs });
                else
                    semanticError("Wrong general arithmetic operation");

                auto res = cfg->createReg(lhs.getType());
                curBlock().addNode(IR_Node(res, std::move(val)));
                return res;
            }
            else if (isIntegerNumOp(expr.op)) {
                if (!ltype.isInteger())
                    semanticError("Operation cannot be applied to non-integer types");

                std::unique_ptr<IR_ExprOper> val;

                if (expr.op == bop::REM)
                    val = std::make_unique<IR_ExprOper>(IR_REM, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::SHL)
                    val = std::make_unique<IR_ExprOper>(IR_SHL, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::SHR)
                    val = std::make_unique<IR_ExprOper>(IR_SHR, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::BIT_XOR)
                    val = std::make_unique<IR_ExprOper>(IR_XOR, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::BIT_AND)
                    val = std::make_unique<IR_ExprOper>(IR_AND, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::BIT_OR)
                    val = std::make_unique<IR_ExprOper>(IR_OR, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::LOG_AND)
                    val = std::make_unique<IR_ExprOper>(IR_LAND, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::LOG_OR)
                    val = std::make_unique<IR_ExprOper>(IR_LOR, std::vector<IRval>{ lhs, rhs });
                else
                    semanticError("Wrong general arithmetic operation");

                auto res = cfg->createReg(lhs.getType());
                curBlock().addNode(IR_Node(res, std::move(val)));
                return res;
            }
            else if (isComparsionOp(expr.op)) {
                std::unique_ptr<IR_ExprOper> val;

                if (expr.op == bop::EQ)
                    val = std::make_unique<IR_ExprOper>(IR_EQ, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::NE)
                    val = std::make_unique<IR_ExprOper>(IR_NE, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::GT)
                    val = std::make_unique<IR_ExprOper>(IR_GT, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::LT)
                    val = std::make_unique<IR_ExprOper>(IR_LT, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::GE)
                    val = std::make_unique<IR_ExprOper>(IR_GE, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::LE)
                    val = std::make_unique<IR_ExprOper>(IR_LE, std::vector<IRval>{ lhs, rhs });
                else
                    semanticError("Wrong comparsion operation");

                auto res = cfg->createReg(std::make_unique<IR_TypeDirect>(IR_TypeDirect::I32));
                curBlock().addNode(IR_Node(res, std::move(val)));
                return res;
            }
            else {
                semanticError("Wrong binary operation");
            }
        }

        case AST_CAST: {
            auto const &expr = dynamic_cast<AST_Cast const &>(node);

            auto arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
            auto dest = getType(*expr.type_name);
            if (arg.getType()->equal(*dest))
                return arg;

            auto val = std::make_unique<IR_ExprCast>(arg, dest);
            auto res = cfg->createReg(dest);
            curBlock().addNode(IR_Node(res, std::move(val)));
            return res;
        }

        case AST_UNARY_OP: {
            using uop = AST_Unop;

            auto const &expr = dynamic_cast<AST_Unop const &>(node);
            auto arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));

            // TODO: check types

            if (expr.op == uop::UN_PLUS) {
                return arg;
            }
            else if (expr.op == uop::UN_MINUS) {
                auto zero =
                        IRval::createVal(std::make_unique<IR_TypeDirect>(IR_TypeDirect::I32), 0U);
                auto val =
                        std::make_unique<IR_ExprOper>(IR_SUB, std::vector<IRval>{ zero, arg });
                auto res = cfg->createReg(arg.getType());
                curBlock().addNode(IR_Node(res, std::move(val)));
                return res;
            }
            else if (expr.op == uop::UN_NEG) {
                // TODO: check this
                auto maxv =
                        IRval::createVal(std::make_unique<IR_TypeDirect>(IR_TypeDirect::I32), (-1U));
                auto val =
                        std::make_unique<IR_ExprOper>(IR_XOR, std::vector<IRval>{ maxv, arg });
                auto res = cfg->createReg(arg.getType());
                curBlock().addNode(IR_Node(res, std::move(val)));
                return res;
            }
            else if (expr.op == uop::UN_NOT) {
                // TODO: check this
                auto maxv =
                        IRval::createVal(std::make_unique<IR_TypeDirect>(IR_TypeDirect::I32), (-1U));
                auto val =
                        std::make_unique<IR_ExprOper>(IR_XOR, std::vector<IRval>{ maxv, arg });
                auto res = cfg->createReg(arg.getType());
                curBlock().addNode(IR_Node(res, std::move(val)));
                return res;
            }
            else {
                NOT_IMPLEMENTED("Unary op");
            }
        }

        case AST_POSTFIX: {
//            auto const &expr = dynamic_cast<AST_Postfix const &>(node);
            NOT_IMPLEMENTED("");
        }

        case AST_PRIMARY: {
            auto const &expr = dynamic_cast<AST_Primary const &>(node);

            if (expr.type == AST_Primary::CONST) {
                AST_Literal val = std::get<AST_Literal>(expr.v);
                auto valType = getLiteralType(val);
                if (val.type == INTEGER_LITERAL) {
                    if (val.isUnsigned) {
                        if (val.longCnt)
                            return IRval::createVal(valType, val.val.vu64);
                        else
                            return IRval::createVal(valType, val.val.vu32);
                    }
                    else { // Signed
                        if (val.longCnt)
                            return IRval::createVal(valType, val.val.vi64);
                        else
                            return IRval::createVal(valType, val.val.vi32);
                    }
                }
                else if (val.type == FLOAT_LITERAL) {
                    if (val.isFloat)
                        return IRval::createVal(valType, val.val.vf32);
                    else {
                        NOT_IMPLEMENTED("double");
                    }
                }
                else if (val.type == CHAR_LITERAL) {
                    return IRval::createVal(valType, static_cast<int8_t>(val.val.v_char));
                }
                else {
                    semanticError("Unknown literal type");
                }
            }
            else if (expr.type == AST_Primary::IDENT) {
                auto var = variables.get(std::get<string_id_t>(expr.v));
                if (!var.has_value())
                    semanticError("Unknown variable");

                auto val = std::make_unique<IR_ExprOper>(
                        IR_DEREF, std::vector<IRval>{ *var });
                auto const &ptrType = dynamic_cast<IR_TypePtr const &>(*var->getType());
                auto res = cfg->createReg(ptrType.child);
                curBlock().addNode(IR_Node(res, std::move(val)));
                return res;
            }
            else if (expr.type == AST_Primary::EXPR) {
                return evalExpr(*std::get<std::unique_ptr<AST_Expr>>(expr.v));
            }

            NOT_IMPLEMENTED("");
        }

        default: {
            semanticError("Unexpected node type in expression");
        }
    }
}
