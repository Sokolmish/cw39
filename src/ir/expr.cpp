#include "generator.hpp"

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
            if (expr.op != AST_Assignment::DIRECT)
                NOT_IMPLEMENTED("compound assignment");
            IRval rhsVal = evalExpr(*expr.rhs);

            if (expr.lhs->node_type == AST_PRIMARY) { // Identifiers
                auto const &assignee = dynamic_cast<AST_Primary const &>(*expr.lhs);
                if (assignee.type != AST_Primary::IDENT)
                    semanticError("Only variables can be assigned");

                auto destVar = variables.get(assignee.getIdent());
                if (!destVar.has_value())
                    semanticError("Unknown variable");
                auto destVarPtrType = std::dynamic_pointer_cast<IR_TypePtr>(destVar->getType());
                if (!destVarPtrType->child->equal(*rhsVal.getType()))
                    semanticError("Cannot assign values of different types");

                curBlock().addNode(IR_Node(std::make_unique<IR_ExprOper>(
                        IR_STORE, std::vector<IRval>{ *destVar, rhsVal })));
            }
            else if (expr.lhs->node_type == AST_UNARY_OP) { // Dereference write
                auto const &assignee = dynamic_cast<AST_Unop const &>(*expr.lhs);
                if (assignee.op != AST_Unop::DEREF)
                    semanticError("Cannot be assigned");

                IRval ptrVal = evalExpr(dynamic_cast<AST_Expr &>(*assignee.child));
                if (ptrVal.getType()->type != IR_Type::POINTER)
                    semanticError("Only pointer type can be dereferenced");
                auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(ptrVal.getType());
                if (ptrType->child->type == IR_Type::FUNCTION)
                    semanticError("Pointer to function cannot be dereferenced");
                if (!ptrType->child->equal(*rhsVal.getType()))
                    semanticError("Cannot assign values of different types");

                IRval res = cfg->createReg(ptrType->child);
                curBlock().addNode(IR_Node(res, std::make_unique<IR_ExprOper>(
                        IR_STORE, std::vector<IRval>{ ptrVal, rhsVal })));
            }
            else if (expr.lhs->node_type == AST_POSTFIX) { // Accessors
                auto const &assignee = dynamic_cast<AST_Postfix const &>(*expr.lhs);
                if (assignee.op == AST_Postfix::DIR_ACCESS) {
                    IRval base = evalExpr(*assignee.base);
                    if (base.getType()->type != IR_Type::TSTRUCT)
                        semanticError("Element access cannot be performed on non-struct type");
                    auto const &structType = dynamic_cast<IR_TypeStruct const &>(*base.getType());
                    auto field = structType.getField(assignee.getIdent());
                    if (field == nullptr)
                        semanticError("Structure has no such field");
                    if (!field->irType->equal(*rhsVal.getType()))
                        semanticError("Cannot assign values of different types");

                    IRval res = cfg->createReg(field->irType);
                    IRval index = IRval::createVal(
                            IR_TypeDirect::type_u64,
                            static_cast<uint64_t>(field->index));
                    curBlock().addNode(IR_Node(res, std::make_unique<IR_ExprOper>(
                            IR_INSERT, std::vector<IRval>{ base, index, rhsVal })));
                }
                else if (assignee.op == AST_Postfix::INDEXATION) {
                    IRval base = evalExpr(*assignee.base);
                    IRval index = evalExpr(assignee.getExpr());

                    if (base.getType()->type != IR_Type::ARRAY)
                        semanticError("Indexation cannot be performed on non-array type");
                    auto arrayType = std::dynamic_pointer_cast<IR_TypeArray>(base.getType());
                    if (!arrayType->child->equal(*rhsVal.getType()))
                        semanticError("Cannot assign values of different types");

                    IRval res = cfg->createReg(arrayType->child);
                    curBlock().addNode(IR_Node(res, std::make_unique<IR_ExprOper>(
                            IR_INSERT, std::vector<IRval>{ base, index, rhsVal })));
                }
                else {
                    semanticError("Only struct's field or array element can be assigned");
                }
            }
            else {
                semanticError("Cannot be assigned");
            }

            return rhsVal;
        }

        case AST_TERNARY: {
            NOT_IMPLEMENTED("ternary");
        }

        case AST_BINOP: {
            using bop = AST_Binop;

            auto const &expr = dynamic_cast<AST_Binop const &>(node);
            IRval lhs = evalExpr(*expr.lhs);
            IRval rhs = evalExpr(*expr.rhs);

            if (!lhs.getType()->equal(*rhs.getType()))
                semanticError("Cannot do binary operation on different types");

            // TODO: pointers arithmetics
            if (lhs.getType()->type != IR_Type::DIRECT)
                NOT_IMPLEMENTED("Pointers arithmetics");

            auto const &ltype = dynamic_cast<IR_TypeDirect const &>(*lhs.getType());
//            auto const &rtype = dynamic_cast<IR_TypeDirect const &>(*rhs.getType());

            if (isGeneralNumOp(expr.op)) {
                std::unique_ptr<IR_ExprOper> opNode;

                if (expr.op == bop::ADD)
                    opNode = std::make_unique<IR_ExprOper>(IR_ADD, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::SUB)
                    opNode = std::make_unique<IR_ExprOper>(IR_SUB, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::MUL)
                    opNode = std::make_unique<IR_ExprOper>(IR_MUL, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::DIV)
                    opNode = std::make_unique<IR_ExprOper>(IR_DIV, std::vector<IRval>{ lhs, rhs });
                else
                    semanticError("Wrong general arithmetic operation");

                auto res = cfg->createReg(lhs.getType());
                curBlock().addNode(IR_Node(res, std::move(opNode)));
                return res;
            }
            else if (isIntegerNumOp(expr.op)) {
                if (!ltype.isInteger())
                    semanticError("Operation cannot be applied to non-integer types");

                std::unique_ptr<IR_ExprOper> opNode;

                if (expr.op == bop::REM)
                    opNode = std::make_unique<IR_ExprOper>(IR_REM, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::SHL)
                    opNode = std::make_unique<IR_ExprOper>(IR_SHL, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::SHR)
                    opNode = std::make_unique<IR_ExprOper>(IR_SHR, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::BIT_XOR)
                    opNode = std::make_unique<IR_ExprOper>(IR_XOR, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::BIT_AND)
                    opNode = std::make_unique<IR_ExprOper>(IR_AND, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::BIT_OR)
                    opNode = std::make_unique<IR_ExprOper>(IR_OR, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::LOG_AND)
                    opNode = std::make_unique<IR_ExprOper>(IR_LAND, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::LOG_OR)
                    opNode = std::make_unique<IR_ExprOper>(IR_LOR, std::vector<IRval>{ lhs, rhs });
                else
                    semanticError("Wrong general arithmetic operation");

                IRval res = cfg->createReg(lhs.getType());
                curBlock().addNode(IR_Node(res, std::move(opNode)));
                return res;
            }
            else if (isComparsionOp(expr.op)) {
                std::unique_ptr<IR_ExprOper> opNode;

                if (expr.op == bop::EQ)
                    opNode = std::make_unique<IR_ExprOper>(IR_EQ, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::NE)
                    opNode = std::make_unique<IR_ExprOper>(IR_NE, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::GT)
                    opNode = std::make_unique<IR_ExprOper>(IR_GT, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::LT)
                    opNode = std::make_unique<IR_ExprOper>(IR_LT, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::GE)
                    opNode = std::make_unique<IR_ExprOper>(IR_GE, std::vector<IRval>{ lhs, rhs });
                else if (expr.op == bop::LE)
                    opNode = std::make_unique<IR_ExprOper>(IR_LE, std::vector<IRval>{ lhs, rhs });
                else
                    semanticError("Wrong comparsion operation");

                auto res = cfg->createReg(IR_TypeDirect::type_i32);
                curBlock().addNode(IR_Node(res, std::move(opNode)));
                return res;
            }
            else {
                semanticError("Wrong binary operation");
            }
        }

        case AST_CAST: {
            auto const &expr = dynamic_cast<AST_Cast const &>(node);

            IRval arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
            auto dest = getType(*expr.type_name);
            if (arg.getType()->equal(*dest))
                return arg;

            IRval res = cfg->createReg(dest);
            curBlock().addNode(IR_Node(res, std::make_unique<IR_ExprCast>(arg, dest)));
            return res;
        }

        case AST_UNARY_OP: {
            using uop = AST_Unop;

            auto const &expr = dynamic_cast<AST_Unop const &>(node);

            if (expr.op == uop::SIZEOF_OP) {
                auto typeName = dynamic_cast<AST_TypeName*>(expr.child.get());
                if (typeName != nullptr) {
                    auto irType = getType(*typeName);
                    uint64_t bytesSize = irType->getBytesSize();
                    return IRval::createVal(IR_TypeDirect::type_u64, bytesSize);
                }
                else {
                    NOT_IMPLEMENTED("Sizeof expression");
                }
            }

            if (expr.op == uop::UN_PLUS) {
                return evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
            }
            else if (expr.op == uop::UN_MINUS) {
                IRval arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
                IRval zero = IRval::createVal(arg.getType(), 0U); // TODO: cast value
                IRval res = cfg->createReg(arg.getType());
                curBlock().addNode(IR_Node(res, std::make_unique<IR_ExprOper>(
                        IR_SUB, std::vector<IRval>{ zero, arg })));
                return res;
            }
            else if (expr.op == uop::UN_NEG) {
                IRval arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
                IRval maxv = IRval::createVal(arg.getType(), -1U); // TODO: cast value
                IRval res = cfg->createReg(arg.getType());
                curBlock().addNode(IR_Node(res, std::make_unique<IR_ExprOper>(
                        IR_XOR, std::vector<IRval>{ maxv, arg })));
                return res;
            }
            else if (expr.op == uop::UN_NOT) {
                IRval arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
                IRval maxv = IRval::createVal(arg.getType(), -1U); // TODO: cast value
                IRval res = cfg->createReg(arg.getType());
                curBlock().addNode(IR_Node(res,  std::make_unique<IR_ExprOper>(
                        IR_XOR, std::vector<IRval>{ maxv, arg })));
                return res;
            }
            else if (expr.op == uop::PRE_INC || expr.op == uop::PRE_DEC) {
                if (expr.child->node_type != AST_PRIMARY)
                    semanticError("Inc/dec available only for primary expressions");
                auto const &prim = dynamic_cast<AST_Primary &>(*expr.child);
                if (prim.type != AST_Primary::IDENT)
                    semanticError("Inc/dec available only for variables");

                IRval arg = evalExpr(dynamic_cast<AST_Expr &>(*expr.child));
                IRval one = IRval::createVal(arg.getType(), 1U); // TODO: cast value
                IRval res = cfg->createReg(arg.getType());
                curBlock().addNode(IR_Node(res, std::make_unique<IR_ExprOper>(
                        expr.op == uop::PRE_INC ? IR_ADD : IR_SUB,
                        std::vector<IRval>{ arg, one })));

                auto var = variables.get(prim.getIdent());
                if (!var.has_value())
                    semanticError("Unknown variable");
                curBlock().addNode(IR_Node(std::make_unique<IR_ExprOper>(
                        IR_STORE, std::vector<IRval>{ *var, res })));

                return res;
            }
            else if (expr.op == uop::DEREF) {
                IRval ptrVal = evalExpr(dynamic_cast<AST_Expr &>(*expr.child));
                if (ptrVal.getType()->type != IR_Type::POINTER)
                    semanticError("Only pointer type can be dereferenced");
                auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(ptrVal.getType());
                if (ptrType->child->type == IR_Type::FUNCTION)
                    semanticError("Pointer to function cannot be dereferenced");

                IRval res = cfg->createReg(ptrType->child);
                curBlock().addNode(IR_Node(res, std::make_unique<IR_ExprOper>(
                        IR_LOAD, std::vector<IRval>{ ptrVal })));
                return res;
            }
            else if (expr.op == uop::ADDR_OF) {
                if (expr.child->node_type == AST_PRIMARY) { // variable
                    auto const &subject = dynamic_cast<AST_Primary const &>(*expr.child);
                    if (subject.type != AST_Primary::IDENT)
                        semanticError("Address cannot be taken from literal");
                    auto var = variables.get(subject.getIdent());
                    if (!var.has_value())
                        semanticError("Unknown variable");
                    return *var;
                }
                else if (expr.child->node_type == AST_POSTFIX) { // Index, access
                    NOT_IMPLEMENTED("Address of aggregate's element");
//                    auto const &subject = dynamic_cast<AST_Postfix const &>(*expr.child);
//                    if (subject.op == AST_Postfix::INDEXATION) {
//                        IRval array = evalExpr(*subject.base);
//                        IRval index = evalExpr(subject.getExpr());
//
//                        if (array.getType()->type != IR_Type::ARRAY)
//                            semanticError("Indexation cannot be performed on non-array type");
//                        auto const &arrayType = dynamic_cast<IR_TypeArray const &>(*array.getType());
//
//                        IRval res = cfg->createReg(std::make_shared<IR_TypePtr>(arrayType.child));
//                        curBlock().addNode(IR_Node(res, std::make_unique<IR_ExprOper>(
//                                IR_GEP, std::vector<IRval>{ array, index })));
//                        return res;
//                    }
//                    else if (subject.op == AST_Postfix::DIR_ACCESS) {
//                        IRval object = evalExpr(*subject.base);
//                        if (object.getType()->type != IR_Type::TSTRUCT)
//                            semanticError("Element access cannot be performed on non-struct type");
//                        auto structType = std::dynamic_pointer_cast<IR_TypeStruct>(object.getType());
//                        auto field = structType->getField(subject.getIdent());
//                        if (field == nullptr)
//                            semanticError("Structure has no such field");
//
//                        IRval res = cfg->createReg(std::make_shared<IR_TypePtr>(field->irType));
//                        IRval index = IRval::createVal(
//                                IR_TypeDirect::type_u64,
//                                static_cast<uint64_t>(field->index));
//                        curBlock().addNode(IR_Node(res, std::make_unique<IR_ExprOper>(
//                                IR_GEP, std::vector<IRval>{ object, index })));
//                        return res;
//                    }
//                    else if (subject.op == AST_Postfix::DIR_ACCESS) {
//                        NOT_IMPLEMENTED("pointer access (->)");
//                    }
//                    else {
//                        semanticError("Cannot take address");
//                    }
                }
                else if (expr.child->node_type == AST_UNARY_OP) { // Deref
                    NOT_IMPLEMENTED("Address of dereference result");
                }
                else if (expr.child->node_type == AST_CAST) { // Cast
                    NOT_IMPLEMENTED("Address of cast result");
                }
                else {
                    semanticError("Cannot take address");
                }
            }
            else {
                semanticError("Unknown unary operator");
            }
        }

        case AST_POSTFIX: {
            auto const &expr = dynamic_cast<AST_Postfix const &>(node);

            if (expr.op == AST_Postfix::CALL) {
                if (expr.base->node_type != AST_PRIMARY)
                    NOT_IMPLEMENTED("Pointers call");
                auto const &funcBase = dynamic_cast<AST_Primary const &>(*expr.base);
                if (funcBase.type != AST_Primary::IDENT)
                    semanticError("Only identifier can be called");
                auto funIt = functions.find(funcBase.getIdent());
                if (funIt == functions.end())
                    semanticError("Call of undeclared function");

                auto const &fun = cfg->getFunction(funIt->second);
                auto const &argsList = expr.getArgsList();

                std::vector<IRval> args;
                size_t argNum = 0;
                for (auto const &arg : argsList.children) {
                    IRval argVal = evalExpr(*arg);

                    if (argNum < fun.getFuncType().args.size()) {
                        if (!argVal.getType()->equal(*fun.getFuncType().args[argNum])) {
                            semanticError("Wrong argnument type");
                        }
                    }
                    else if (!fun.getFuncType().isVariadic) {
                        semanticError("Too manny arguments in function call");
                    }

                    args.push_back(argVal);
                    argNum++;
                }
                if (argNum < fun.getFuncType().args.size()) {
                    semanticError("Too few argument in function call");
                }

                IRval res = cfg->createReg(fun.getFuncType().ret);
                curBlock().addNode(IR_Node(res, std::make_unique<IR_ExprCall>(fun.getId(), args)));
                return res;
            }
            else if (expr.op == AST_Postfix::POST_INC || expr.op == AST_Postfix::POST_DEC) {
                if (expr.base->node_type != AST_PRIMARY)
                    semanticError("Inc/dec available only for primary expressions");
                auto const &prim = dynamic_cast<AST_Primary const &>(*expr.base);
                if (prim.type != AST_Primary::IDENT)
                    semanticError("Inc/dec available only for variables");

                IRval arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.base));
                IRval one = IRval::createVal(arg.getType(), 1U); // TODO: cast value
                IRval res = cfg->createReg(arg.getType());
                curBlock().addNode(IR_Node(res, std::make_unique<IR_ExprOper>(
                        expr.op == AST_Postfix::POST_INC ? IR_ADD : IR_SUB,
                        std::vector<IRval>{ arg, one })));

                auto var = variables.get(prim.getIdent());
                if (!var.has_value())
                    semanticError("Unknown variable");
                curBlock().addNode(IR_Node(std::make_unique<IR_ExprOper>(
                        IR_STORE, std::vector<IRval>{ *var, res })));
                return arg;
            }
            else if (expr.op == AST_Postfix::INDEXATION) {
                IRval array = evalExpr(*expr.base);
                IRval index = evalExpr(expr.getExpr());

                if (array.getType()->type != IR_Type::ARRAY)
                    semanticError("Indexation cannot be performed on non-array type");
                auto const &arrayType = dynamic_cast<IR_TypeArray const &>(*array.getType());

                IRval res = cfg->createReg(arrayType.child);
                curBlock().addNode(IR_Node(res, std::make_unique<IR_ExprOper>(
                        IR_EXTRACT, std::vector<IRval>{ array, index })));
                return res;
            }
            else if (expr.op == AST_Postfix::DIR_ACCESS) {
                IRval object = evalExpr(*expr.base);
                if (object.getType()->type != IR_Type::TSTRUCT)
                    semanticError("Element access cannot be performed on non-struct type");
                auto structType = std::dynamic_pointer_cast<IR_TypeStruct>(object.getType());
                auto field = structType->getField(expr.getIdent());
                if (field == nullptr)
                    semanticError("Structure has no such field");

                IRval res = cfg->createReg(field->irType);
                IRval index = IRval::createVal(
                        IR_TypeDirect::type_u64,
                        static_cast<uint64_t>(field->index));
                curBlock().addNode(IR_Node(res, std::make_unique<IR_ExprOper>(
                        IR_EXTRACT, std::vector<IRval>{ object, index })));
                return res;
            }
            else if (expr.op == AST_Postfix::PTR_ACCESS) {
                NOT_IMPLEMENTED("pointer access (->)");
            }
            else {
                NOT_IMPLEMENTED("Postfix expression");
            }
        }

        case AST_PRIMARY: {
            auto const &expr = dynamic_cast<AST_Primary const &>(node);

            if (expr.type == AST_Primary::CONST) {
                AST_Literal val = expr.getLiteral();
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
                auto var = variables.get(expr.getIdent());
                if (!var.has_value()) {
                    semanticError("Unknown variable");
                }

                auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(var->getType());
                IRval res = cfg->createReg(ptrType->child);
                curBlock().addNode(IR_Node(res, std::make_unique<IR_ExprOper>(
                        IR_LOAD, std::vector<IRval>{ *var })));
                return res;
            }
            else if (expr.type == AST_Primary::EXPR) {
                return evalExpr(expr.getExpr());
            }
            else if (expr.type == AST_Primary::STR) {
                string_id_t parserStrId = expr.getString();
                IRval str = IRval::createString(cfg->putString(get_string_by_id(parserStrId)));
                strings.insert({ parserStrId, str });
                return str;
            }
            else {
                semanticError("Unexpected primary type");
            }
        }

        default: {
            semanticError("Unexpected node type in expression");
        }
    }
}
