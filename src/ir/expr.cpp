#include "generator.hpp"

void IR_Generator::doAssignment(AST_Expr const &dest, IRval wrValue) {
    if (dest.node_type == AST_PRIMARY) { // Identifiers
        auto const &assignee = dynamic_cast<AST_Primary const &>(dest);
        if (assignee.type == AST_Primary::EXPR) {
            doAssignment(assignee.getExpr(), wrValue);
            return;
        }
        else if (assignee.type != AST_Primary::IDENT) {
            semanticError("Only variables can be assigned");
        }

        auto destVar = variables.get(assignee.getIdent());
        auto destGlobal = globals.find(assignee.getIdent());
        if (destVar.has_value()) {
            auto destVarPtrType = std::dynamic_pointer_cast<IR_TypePtr>(destVar->getType());
            if (!destVarPtrType->child->equal(*wrValue.getType()))
                semanticError("Cannot assign values of different types");
            curBlock().addOperNode({}, IR_ExprOper::STORE, { *destVar, wrValue });
        }
        else if (destGlobal != globals.end()) {
            auto destVarPtrType = std::dynamic_pointer_cast<IR_TypePtr>(
                    destGlobal->second.getType());
            if (!destVarPtrType->child->equal(*wrValue.getType()))
                semanticError("Cannot assign values of different types");
            curBlock().addOperNode({}, IR_ExprOper::STORE, { destGlobal->second, wrValue });
        }
        else {
            semanticError("Unknown variable");
        }
    }
    else if (dest.node_type == AST_UNARY_OP) { // Dereference write
        auto const &assignee = dynamic_cast<AST_Unop const &>(dest);
        if (assignee.op != AST_Unop::DEREF)
            semanticError("Cannot be assigned");

        IRval ptrVal = evalExpr(dynamic_cast<AST_Expr &>(*assignee.child));
        if (ptrVal.getType()->type != IR_Type::POINTER)
            semanticError("Only pointer type can be dereferenced");
        auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(ptrVal.getType());
        if (ptrType->child->type == IR_Type::FUNCTION)
            semanticError("Pointer to function cannot be dereferenced");
        if (!ptrType->child->equal(*wrValue.getType()))
            semanticError("Cannot assign values of different types");

        curBlock().addOperNode({}, IR_ExprOper::STORE, { ptrVal, wrValue });
    }
    else if (dest.node_type == AST_POSTFIX) { // Accessors
        auto const &assignee = dynamic_cast<AST_Postfix const &>(dest);
        if (assignee.op == AST_Postfix::DIR_ACCESS) {
            IRval base = evalExpr(*assignee.base);
            if (base.getType()->type != IR_Type::TSTRUCT)
                semanticError("Element access cannot be performed on non-struct type");
            auto const &structType = dynamic_cast<IR_TypeStruct const &>(*base.getType());
            auto field = structType.getField(assignee.getIdent());
            if (field == nullptr)
                semanticError("Structure has no such field");
            if (!field->irType->equal(*wrValue.getType()))
                semanticError("Cannot assign values of different types");

            IRval res = cfg->createReg(base.getType());
            IRval index = IRval::createVal(
                    IR_TypeDirect::type_u64,
                    static_cast<uint64_t>(field->index));
            curBlock().addOperNode(res, IR_ExprOper::INSERT, { base, index, wrValue });

            doAssignment(*assignee.base, res);
        }
        else if (assignee.op == AST_Postfix::PTR_ACCESS) {
            NOT_IMPLEMENTED("Indirect struct access");
        }
        else if (assignee.op == AST_Postfix::INDEXATION) {
            IRval base = evalExpr(*assignee.base);
            IRval index = evalExpr(assignee.getExpr());

            if (base.getType()->type == IR_Type::ARRAY) { // Not used?
                auto arrayType = std::dynamic_pointer_cast<IR_TypeArray>(base.getType());
                if (!arrayType->child->equal(*wrValue.getType()))
                    semanticError("Cannot assign values of different types");

                curBlock().addNode(IR_Node(std::make_unique<IR_ExprOper>( // TODO: no res value (and unused)
                        IR_ExprOper::INSERT, std::vector<IRval>{ base, index, wrValue })));
            }
            else if (base.getType()->type == IR_Type::POINTER) {
                auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(base.getType());
                if (!ptrType->child->equal(*wrValue.getType()))
                    semanticError("Cannot assign values of different types");

                IRval fixedIndex = index;
                if (!fixedIndex.getType()->equal(*IR_TypeDirect::type_u64)) {
                    fixedIndex = cfg->createReg(IR_TypeDirect::type_u64);
                    curBlock().addCastNode(fixedIndex, index, IR_TypeDirect::type_u64);
                }

                IRval scaledIndex = fixedIndex;
                if (ptrType->child->getBytesSize() != 1) {
                    IRval multiplyer = IRval::createVal(
                            IR_TypeDirect::type_u64,
                            static_cast<uint64_t>(ptrType->child->getBytesSize()));
                    scaledIndex = cfg->createReg(fixedIndex.getType());
                    curBlock().addOperNode(scaledIndex, IR_ExprOper::MUL, { fixedIndex, multiplyer });
                }

                IRval iptr = cfg->createReg(IR_TypeDirect::type_u64);
                curBlock().addCastNode(iptr, base, IR_TypeDirect::type_u64);
                IRval wOffset = cfg->createReg(IR_TypeDirect::type_u64);
                curBlock().addOperNode(wOffset, IR_ExprOper::ADD, { iptr, scaledIndex });
                IRval finPtr = cfg->createReg(ptrType);
                curBlock().addCastNode(finPtr, wOffset, ptrType);
                curBlock().addOperNode({}, IR_ExprOper::STORE, { finPtr, wrValue });
            }
            else {
                semanticError("Wrong type for indexation");
            }
        }
        else {
            semanticError("Only struct's field or array element can be assigned");
        }
    }
    else {
        semanticError("Cannot be assigned");
    }
}

IRval IR_Generator::doBinOp(AST_Binop::OpType op, IRval const &lhs, IRval const &rhs) {
    using bop = AST_Binop;

    if (lhs.getType()->type != IR_Type::DIRECT)
        NOT_IMPLEMENTED("Pointers arithmetics");

    if (!lhs.getType()->equal(*rhs.getType()))
        semanticError("Cannot do binary operation on different types");

    auto const &ltype = dynamic_cast<IR_TypeDirect const &>(*lhs.getType());
//    auto const &rtype = dynamic_cast<IR_TypeDirect const &>(*rhs.getType());

    if (isGeneralNumOp(op)) {
        IRval res = cfg->createReg(lhs.getType());

        if (op == bop::ADD)
            curBlock().addOperNode(res, IR_ExprOper::ADD, { lhs, rhs });
        else if (op == bop::SUB)
            curBlock().addOperNode(res, IR_ExprOper::SUB, { lhs, rhs });
        else if (op == bop::MUL)
            curBlock().addOperNode(res, IR_ExprOper::MUL, { lhs, rhs });
        else if (op == bop::DIV)
            curBlock().addOperNode(res, IR_ExprOper::DIV, { lhs, rhs });
        else
            semanticError("Wrong general arithmetic operation");

        return res;
    }
    else if (isIntegerNumOp(op)) {
        if (!ltype.isInteger())
            semanticError("Operation cannot be applied to non-integer types");

        IRval res = cfg->createReg(lhs.getType());

        if (op == bop::REM)
            curBlock().addOperNode(res, IR_ExprOper::REM, { lhs, rhs });
        else if (op == bop::SHL)
            curBlock().addOperNode(res, IR_ExprOper::SHL, { lhs, rhs });
        else if (op == bop::SHR)
            curBlock().addOperNode(res, IR_ExprOper::SHR, { lhs, rhs });
        else if (op == bop::BIT_XOR)
            curBlock().addOperNode(res, IR_ExprOper::XOR, { lhs, rhs });
        else if (op == bop::BIT_AND)
            curBlock().addOperNode(res, IR_ExprOper::AND, { lhs, rhs });
        else if (op == bop::BIT_OR)
            curBlock().addOperNode(res, IR_ExprOper::OR, { lhs, rhs });
        else if (op == bop::LOG_AND || op == bop::LOG_OR)
            semanticError("Logical operations in general context");
        else
            semanticError("Wrong general arithmetic operation");

        return res;
    }
    else if (isComparsionOp(op)) {
        IRval res = cfg->createReg(IR_TypeDirect::type_i32);

        if (op == bop::EQ)
            curBlock().addOperNode(res, IR_ExprOper::EQ, { lhs, rhs });
        else if (op == bop::NE)
            curBlock().addOperNode(res, IR_ExprOper::NE, { lhs, rhs });
        else if (op == bop::GT)
            curBlock().addOperNode(res, IR_ExprOper::GT, { lhs, rhs });
        else if (op == bop::LT)
            curBlock().addOperNode(res, IR_ExprOper::LT, { lhs, rhs });
        else if (op == bop::GE)
            curBlock().addOperNode(res, IR_ExprOper::GE, { lhs, rhs });
        else if (op == bop::LE)
            curBlock().addOperNode(res, IR_ExprOper::LE, { lhs, rhs });
        else
            semanticError("Wrong comparsion operation");

        return res;
    }
    else {
        semanticError("Wrong binary operation");
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
            IRval rhsVal = evalExpr(*expr.rhs);
            if (expr.op != AST_Assignment::DIRECT) {
                IRval lhsVal = evalExpr(*expr.lhs);
                switch (expr.op) {
                    case AST_Assignment::MUL:
                        rhsVal = doBinOp(AST_Binop::MUL, lhsVal, rhsVal);
                        break;
                    case AST_Assignment::DIV:
                        rhsVal = doBinOp(AST_Binop::DIV, lhsVal, rhsVal);
                        break;
                    case AST_Assignment::REM:
                        rhsVal = doBinOp(AST_Binop::REM, lhsVal, rhsVal);
                        break;
                    case AST_Assignment::ADD:
                        rhsVal = doBinOp(AST_Binop::ADD, lhsVal, rhsVal);
                        break;
                    case AST_Assignment::SUB:
                        rhsVal = doBinOp(AST_Binop::SUB, lhsVal, rhsVal);
                        break;
                    case AST_Assignment::SHL:
                        rhsVal = doBinOp(AST_Binop::SHL, lhsVal, rhsVal);
                        break;
                    case AST_Assignment::SHR:
                        rhsVal = doBinOp(AST_Binop::SHR, lhsVal, rhsVal);
                        break;
                    case AST_Assignment::AND:
                        rhsVal = doBinOp(AST_Binop::BIT_AND, lhsVal, rhsVal);
                        break;
                    case AST_Assignment::XOR:
                        rhsVal = doBinOp(AST_Binop::BIT_XOR, lhsVal, rhsVal);
                        break;
                    case AST_Assignment::OR:
                        rhsVal = doBinOp(AST_Binop::BIT_OR, lhsVal, rhsVal);
                        break;
                    default:
                        semanticError("Wrong assignment type");
                }
            }
            doAssignment(*expr.lhs, rhsVal);
            return rhsVal;
        }

        case AST_TERNARY: {
            NOT_IMPLEMENTED("ternary");
        }

        case AST_BINOP: {
            auto const &expr = dynamic_cast<AST_Binop const &>(node);

            if (!isInList(expr.op, { AST_Binop::LOG_AND, AST_Binop::LOG_OR })) {
                IRval lhs = evalExpr(*expr.lhs);
                IRval rhs = evalExpr(*expr.rhs);
                return doBinOp(expr.op, lhs, rhs);
            }
            else { // Logical operation (with short evaluation)
                IRval lhs = evalExpr(*expr.lhs);

                if (lhs.getType()->type != IR_Type::DIRECT)
                    semanticError("Cannon perform logical operation on non-integer type");
                auto const &ltype = dynamic_cast<IR_TypeDirect const &>(*lhs.getType());
                if (!ltype.isInteger())
                    semanticError("Cannon perform logical operation on non-integer type");

                IRval res = cfg->createReg(lhs.getType());
                curBlock().addOperNode(res, IR_ExprOper::MOV, { lhs });

                IR_Block &blockLong = cfg->createBlock();
                IR_Block &blockAfter = cfg->createBlock();

                curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
                        IR_ExprTerminator::BRANCH, lhs));
                if (expr.op == AST_Binop::LOG_AND) {
                    cfg->linkBlocks(curBlock(), blockLong);
                    cfg->linkBlocks(curBlock(), blockAfter);
                }
                else {
                    cfg->linkBlocks(curBlock(), blockAfter);
                    cfg->linkBlocks(curBlock(), blockLong);
                }

                selectBlock(blockLong);
                IRval rhs = evalExpr(*expr.rhs);

                if (!lhs.getType()->equal(*rhs.getType()))
                    semanticError("Cannot do binary operation on different types");

                curBlock().addOperNode(res, IR_ExprOper::MOV, { rhs });

                curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
                        IR_ExprTerminator::JUMP, lhs));
                cfg->linkBlocks(curBlock(), blockAfter);

                selectBlock(blockAfter);
                return res; // Maybe PHI node here?
            }
        }

        case AST_CAST: {
            auto const &expr = dynamic_cast<AST_Cast const &>(node);

            IRval arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
            auto dest = getType(*expr.type_name);
            if (arg.getType()->equal(*dest))
                return arg;

            IRval res = cfg->createReg(dest);
            curBlock().addCastNode(res, arg, dest);
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
                IRval zero = IRval::createVal(arg.getType(), 0U);
                IRval res = cfg->createReg(arg.getType());
                curBlock().addOperNode(res, IR_ExprOper::SUB, { zero, arg });
                return res;
            }
            else if (expr.op == uop::UN_NEG) {
                IRval arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
                IRval maxv = IRval::createVal(arg.getType(), -1UL);
                IRval res = cfg->createReg(arg.getType());
                curBlock().addOperNode(res, IR_ExprOper::XOR, { maxv, arg });
                return res;
            }
            else if (expr.op == uop::UN_NOT) { // TODO: fix
                IRval arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
                IRval maxv = IRval::createVal(arg.getType(), -1UL);
                IRval res = cfg->createReg(arg.getType());
                curBlock().addOperNode(res, IR_ExprOper::XOR, { maxv, arg });
                return res;
            }
            else if (expr.op == uop::PRE_INC || expr.op == uop::PRE_DEC) {
                if (expr.child->node_type != AST_PRIMARY) // TODO: use assignment
                    semanticError("Inc/dec available only for primary expressions");
                auto const &prim = dynamic_cast<AST_Primary &>(*expr.child);
                if (prim.type != AST_Primary::IDENT)
                    semanticError("Inc/dec available only for variables");

                IRval arg = evalExpr(dynamic_cast<AST_Expr &>(*expr.child));
                IRval one = IRval::createVal(arg.getType(), 1U);
                IRval res = cfg->createReg(arg.getType());
                auto op = expr.op == uop::PRE_INC ? IR_ExprOper::ADD : IR_ExprOper::SUB;
                curBlock().addOperNode(res, op, { arg, one });

                auto var = variables.get(prim.getIdent());
                if (!var.has_value())
                    semanticError("Unknown variable");
                curBlock().addOperNode({}, IR_ExprOper::STORE, { *var, res });

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
                curBlock().addOperNode(res, IR_ExprOper::LOAD, { ptrVal });
                return res;
            }
            else if (expr.op == uop::ADDR_OF) {
                if (expr.child->node_type == AST_PRIMARY) { // variable
                    auto const &subject = dynamic_cast<AST_Primary const &>(*expr.child);
                    if (subject.type != AST_Primary::IDENT)
                        semanticError("Address cannot be taken from literal");

                    auto var = variables.get(subject.getIdent());
                    auto globalIt = globals.find(subject.getIdent());
                    if (var.has_value())
                        return *var;
                    else if (globalIt != globals.end())
                        return globalIt->second;
                    else
                        semanticError("Unknown variable");
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
//                                IR_GEP, std::vector<IRval>{ array, index }))); // addOperNode
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
//                                IR_GEP, std::vector<IRval>{ object, index }))); // addOperNode
//                        return res;
//                    }
//                    else if (subject.op == AST_Postfix::DIR_ACCESS) {
//                        NOT_IMPLEMENTED("pointer access (->)");
//                    }
//                    else {
//                        semanticError("Cannot take address");
//                    }
                }
                else if (expr.child->node_type == AST_UNARY_OP) { // Deref // TODO
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
                curBlock().addCallNode(res, fun.getId(), args);
                return res;
            }
            else if (expr.op == AST_Postfix::POST_INC || expr.op == AST_Postfix::POST_DEC) {
                if (expr.base->node_type != AST_PRIMARY)
                    semanticError("Inc/dec available only for primary expressions");
                auto const &prim = dynamic_cast<AST_Primary const &>(*expr.base);
                if (prim.type != AST_Primary::IDENT)
                    semanticError("Inc/dec available only for variables");

                IRval arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.base));
                IRval one = IRval::createVal(arg.getType(), 1U);
                IRval res = cfg->createReg(arg.getType());
                auto op = expr.op == AST_Postfix::POST_INC ? IR_ExprOper::ADD : IR_ExprOper::SUB;
                curBlock().addOperNode(res, op, { arg, one });

                auto var = variables.get(prim.getIdent());
                if (!var.has_value())
                    semanticError("Unknown variable");
                curBlock().addOperNode({}, IR_ExprOper::STORE, { *var, res });
                return arg;
            }
            else if (expr.op == AST_Postfix::INDEXATION) {
                IRval array = evalExpr(*expr.base);
                IRval index = evalExpr(expr.getExpr());

                if (array.getType()->type == IR_Type::ARRAY) {
                    auto const &arrayType = dynamic_cast<IR_TypeArray const &>(*array.getType());
                    IRval res = cfg->createReg(arrayType.child);
                    curBlock().addOperNode(res, IR_ExprOper::EXTRACT, { array, index });
                    return res;
                }
                else if (array.getType()->type == IR_Type::POINTER) {
                    // TODO: common code for this indexation with assignment
                    auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(array.getType());

                    IRval fixedIndex = index;
                    if (!fixedIndex.getType()->equal(*IR_TypeDirect::type_u64)) {
                        fixedIndex = cfg->createReg(IR_TypeDirect::type_u64);
                        curBlock().addCastNode(fixedIndex, index, IR_TypeDirect::type_u64);
                    }

                    IRval scaledIndex = fixedIndex;
                    if (ptrType->child->getBytesSize() != 1) {
                        IRval multiplyer = IRval::createVal(
                                IR_TypeDirect::type_u64,
                                static_cast<uint64_t>(ptrType->child->getBytesSize()));
                        scaledIndex = cfg->createReg(fixedIndex.getType());
                        curBlock().addOperNode(scaledIndex, IR_ExprOper::MUL, { fixedIndex, multiplyer });
                    }

                    IRval iptr = cfg->createReg(IR_TypeDirect::type_u64);
                    curBlock().addCastNode(iptr, array, IR_TypeDirect::type_u64);
                    IRval wOffset = cfg->createReg(IR_TypeDirect::type_u64);
                    curBlock().addOperNode(wOffset, IR_ExprOper::ADD, { iptr, scaledIndex });
                    IRval finPtr = cfg->createReg(ptrType);
                    curBlock().addCastNode(finPtr, wOffset, ptrType);
                    IRval res = cfg->createReg(ptrType->child);
                    curBlock().addOperNode(res, IR_ExprOper::LOAD, { finPtr });
                    return res;
                }
                else {
                    semanticError("Indexation cannot be performed on non-array type");
                }
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
                curBlock().addOperNode(res, IR_ExprOper::EXTRACT, { object, index });
                return res;
            }
            else if (expr.op == AST_Postfix::PTR_ACCESS) {
                NOT_IMPLEMENTED("pointer access (->)");
            }
            else {
                semanticError("Unknown postfix expression");
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
                        return IRval::createVal(valType, val.val.vf64);
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
                std::vector<IRval> ptrArg;
                std::shared_ptr<IR_Type> resType;

                auto var = variables.get(expr.getIdent());
                auto globalIt = globals.find(expr.getIdent());
                if (var.has_value()) {
                    auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(var->getType());
                    resType = ptrType->child;
                    ptrArg = { *var };
                }
                else if (globalIt != globals.end()) {
                    auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(
                            globalIt->second.getType());
                    resType = ptrType->child;
                    ptrArg = { globalIt->second };
                }
                else {
                    semanticError("Unknown variable");
                }

                if (resType->type == IR_Type::ARRAY) {
                    auto const &arrType = dynamic_cast<IR_TypeArray const &>(*resType);
                    auto ptrType = std::make_shared<IR_TypePtr>(arrType.child);
                    IRval res = cfg->createReg(ptrType);
                    curBlock().addCastNode(res, ptrArg[0], ptrType);
                    return res;
                }
                else {
                    IRval res = cfg->createReg(resType);
                    curBlock().addOperNode(res, IR_ExprOper::LOAD, ptrArg);
                    return res;
                }
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
