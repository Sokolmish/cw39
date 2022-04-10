#include "generator.hpp"

bool IR_Generator::isGeneralNumOp(AST_Binop::OpType op) {
    using b = AST_Binop;
    return isInList(op, { b::ADD, b::SUB, b::DIV, b::MUL });
}

bool IR_Generator::isIntegerNumOp(AST_Binop::OpType op) {
    using b = AST_Binop;
    return isInList(op, { b::SHL, b::SHR, b::REM, b::BIT_AND, b::BIT_OR, b::BIT_XOR });
}

bool IR_Generator::isComparsionOp(AST_Binop::OpType op) {
    using b = AST_Binop;
    return isInList(op, { b::LT, b::GT, b::LE, b::GE, b::EQ, b::NE, b::LOG_AND, b::LOG_OR });
}


std::optional<IRval> IR_Generator::evalConstantExpr(AST_Expr const &node) {
    bool lastShortLogicState = isShortLogicEnabled;
    isShortLogicEnabled = false;
    std::optional<IRval> val = evalExpr(node);
    isShortLogicEnabled = lastShortLogicState;
    if (val.has_value() && val->getValueClass() == IRval::VAL)
        return val;
    else
        return {};
}


/** Creates pointer to element with given index in array pointed to by base */
IRval IR_Generator::getPtrWithOffset(IRval const &base, IRval const &index) {
    auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(base.getType());

    // TODO: use GEP

    IRval fixedIndex = index;
    if (!fixedIndex.getType()->equal(*IR_TypeDirect::getU64()))
        fixedIndex = emitCast(index, IR_TypeDirect::getU64());

    IRval scaledIndex = fixedIndex;
    if (ptrType->child->getBytesSize() != 1) {
        IRval mult = IRval::createVal(IR_TypeDirect::getU64(),
                                      static_cast<uint64_t>(ptrType->child->getBytesSize()));
        scaledIndex = emitOp(fixedIndex.getType(), IR_ExprOper::MUL, { fixedIndex, mult });
    }

    IRval iptr = emitCast(base, IR_TypeDirect::getU64());
    IRval wOffset = emitOp(IR_TypeDirect::getU64(), IR_ExprOper::ADD, { iptr, scaledIndex });
    return emitCast(wOffset, ptrType);
}

/** Store wrValue in object described by dest (variable, pointer, field, etc) */
void IR_Generator::doAssignment(AST_Expr const &dest, IRval const &wrValue) {
    if (dest.node_type == AST_PRIMARY) { // Identifiers
        auto const &assignee = static_cast<AST_Primary const &>(dest);
        if (assignee.type == AST_Primary::EXPR) {
            doAssignment(assignee.getExpr(), wrValue);
            return;
        }
        else if (assignee.type != AST_Primary::IDENT) {
            semanticError(assignee.loc, "Only variables can be assigned");
        }

        std::optional<IRval> destVar = getPtrToVariable(assignee.getIdent());
        if (!destVar.has_value())
            semanticError(assignee.loc, "Unknown variable");
        auto destVarPtrType = std::dynamic_pointer_cast<IR_TypePtr>(destVar->getType());
        if (!destVarPtrType->child->equal(*wrValue.getType()))
            semanticError(assignee.loc, "Cannot assign values of different types");
        emitStore(*destVar, wrValue);
    }
    else if (dest.node_type == AST_UNARY_OP) { // Dereference write
        auto const &assignee = static_cast<AST_Unop const &>(dest);
        if (assignee.op != AST_Unop::DEREF)
            semanticError(assignee.loc, "Cannot be assigned");

        IRval ptrVal = evalExpr(dynamic_cast<AST_Expr &>(*assignee.child));
        if (ptrVal.getType()->type != IR_Type::POINTER)
            semanticError(assignee.loc, "Only pointer type can be dereferenced");
        auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(ptrVal.getType());
        if (ptrType->child->type == IR_Type::FUNCTION)
            semanticError(assignee.loc, "Pointer to function cannot be dereferenced");
        if (!ptrType->child->equal(*wrValue.getType()))
            semanticError(assignee.loc, "Cannot assign values of different types");

        emitStore(ptrVal, wrValue);
    }
    else if (dest.node_type == AST_POSTFIX) { // Accessors
        auto const &assignee = static_cast<AST_Postfix const &>(dest);
        if (assignee.op == AST_Postfix::DIR_ACCESS) {
            IRval base = evalExpr(*assignee.base);
            if (base.getType()->type != IR_Type::TSTRUCT)
                semanticError(assignee.loc, "Element access cannot be performed on non-struct type");
            auto const &structType = dynamic_cast<IR_TypeStruct const &>(*base.getType());
            auto field = structType.getField(assignee.getIdent());
            if (field == nullptr)
                semanticError(assignee.loc, "Structure has no such field");
            if (!field->irType->equal(*wrValue.getType()))
                semanticError(assignee.loc, "Cannot assign values of different types");

            IRval index = IRval::createVal(IR_TypeDirect::getU64(),
                                           static_cast<uint64_t>(field->index));
            IRval res = emitInsert(base.getType(), base, wrValue, { index }); // TODO: GEP

            doAssignment(*assignee.base, res);
        }
        else if (assignee.op == AST_Postfix::PTR_ACCESS) {
            // TODO: common code for PTR_ACCESS
            IRval object = evalExpr(*assignee.base);
            if (object.getType()->type != IR_Type::POINTER)
                semanticError(assignee.loc, "Pointer access cannot be performed on non-pointer type");
            auto structPtrType = std::dynamic_pointer_cast<IR_TypePtr>(object.getType());
            if (structPtrType->child->type != IR_Type::TSTRUCT)
                semanticError(assignee.loc, "Element access cannot be performed on non-struct type");
            auto structType = std::dynamic_pointer_cast<IR_TypeStruct>(structPtrType->child);
            auto field = structType->getField(assignee.getIdent());
            if (field == nullptr)
                semanticError(assignee.loc, "Structure has no such field");

            IRval index = IRval::createVal(IR_TypeDirect::getI32(),
                                           static_cast<int32_t>(field->index));
            auto fieldPtrType = std::make_shared<IR_TypePtr>(field->irType);
            IRval elemPtr = emitGEP(fieldPtrType, std::move(object), {
                    IRval::createVal(IR_TypeDirect::getI32(), 0), std::move(index) });
            emitStore(std::move(elemPtr), wrValue);
        }
        else if (assignee.op == AST_Postfix::INDEXATION) {
            IRval base = evalExpr(*assignee.base);
            IRval index = evalExpr(assignee.getExpr());

            if (base.getType()->type == IR_Type::ARRAY) {
//                auto arrayType = std::dynamic_pointer_cast<IR_TypeArray>(base.getType());
//                if (!arrayType->child->equal(*wrValue.getType()))
//                    semanticError("Cannot assign values of different types");
//
//                curBlock().addNode(IR_Node(std::make_unique<IR_ExprOper>(
//                        IR_ExprOper::INSERT, std::vector<IRval>{ base, index, wrValue })));
                internalError("Something went wrong");
            }
            else if (base.getType()->type == IR_Type::POINTER) {
                auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(base.getType());
                if (!ptrType->child->equal(*wrValue.getType()))
                    semanticError(assignee.loc, "Cannot assign values of different types");
                IRval finPtr = getPtrWithOffset(base, index);
                emitStore(finPtr, wrValue);
            }
            else { // base.getType()->type
                semanticError(assignee.loc, "Wrong type for indexation");
            }
        }
        else { // assignee.op
            semanticError(assignee.loc, "Only struct's field or array element can be assigned");
        }
    }
    else { // dest.node_type
        semanticError(dest.loc, "Cannot be assigned");
    }
}

/** Create node with specified binary operation */
IRval IR_Generator::doBinOp(AST_Binop::OpType op, IRval const &lhs, IRval const &rhs, yy::location loc) {
    using bop = AST_Binop;

    if (lhs.getType()->type != IR_Type::DIRECT)
        semanticError(loc, "Wrong arithmetic type");
    if (!lhs.getType()->equal(*rhs.getType()))
        semanticError(loc, "Cannot do binary operation on different types");

    auto const &ltype = dynamic_cast<IR_TypeDirect const &>(*lhs.getType());

    if (isGeneralNumOp(op)) {
        if (op == bop::ADD)
            return emitOp(lhs.getType(), IR_ExprOper::ADD, { lhs, rhs });
        else if (op == bop::SUB)
            return emitOp(lhs.getType(), IR_ExprOper::SUB, { lhs, rhs });
        else if (op == bop::MUL)
            return emitOp(lhs.getType(), IR_ExprOper::MUL, { lhs, rhs });
        else if (op == bop::DIV)
            return emitOp(lhs.getType(), IR_ExprOper::DIV, { lhs, rhs });
        else
            internalError("Wrong general arithmetic operation");
    }
    else if (isIntegerNumOp(op)) {
        if (!ltype.isInteger())
            semanticError(loc, "Operation cannot be applied to non-integer types");

        if (op == bop::REM)
            return emitOp(lhs.getType(), IR_ExprOper::REM, { lhs, rhs });
        else if (op == bop::SHL)
            return emitOp(lhs.getType(), IR_ExprOper::SHL, { lhs, rhs });
        else if (op == bop::SHR)
            return emitOp(lhs.getType(), IR_ExprOper::SHR, { lhs, rhs });
        else if (op == bop::BIT_XOR)
            return emitOp(lhs.getType(), IR_ExprOper::XOR, { lhs, rhs });
        else if (op == bop::BIT_AND)
            return emitOp(lhs.getType(), IR_ExprOper::AND, { lhs, rhs });
        else if (op == bop::BIT_OR)
            return emitOp(lhs.getType(), IR_ExprOper::OR, { lhs, rhs });
        else
            internalError("Wrong general arithmetic operation");
    }
    else if (isComparsionOp(op)) {
        if (op == bop::EQ)
            return emitOp(IR_TypeDirect::getI1(), IR_ExprOper::EQ, { lhs, rhs });
        else if (op == bop::NE)
            return emitOp(IR_TypeDirect::getI1(), IR_ExprOper::NE, { lhs, rhs });
        else if (op == bop::GT)
            return emitOp(IR_TypeDirect::getI1(), IR_ExprOper::GT, { lhs, rhs });
        else if (op == bop::LT)
            return emitOp(IR_TypeDirect::getI1(), IR_ExprOper::LT, { lhs, rhs });
        else if (op == bop::GE)
            return emitOp(IR_TypeDirect::getI1(), IR_ExprOper::GE, { lhs, rhs });
        else if (op == bop::LE)
            return emitOp(IR_TypeDirect::getI1(), IR_ExprOper::LE, { lhs, rhs });
        else if (op == bop::LOG_AND)
            return emitOp(IR_TypeDirect::getI1(), IR_ExprOper::LAND, { lhs, rhs });
        else if (op == bop::LOG_OR)
            return emitOp(IR_TypeDirect::getI1(), IR_ExprOper::LOR, { lhs, rhs });
        else
            internalError("Wrong comparsion operation");
    }
    else {
        internalError("Wrong binary operation");
    }
}

/** Create nodes and blocks for logical operation with short evaluation */
IRval IR_Generator::doShortLogicOp(AST_Binop::OpType op, AST_Expr const &left, AST_Expr const &right,
                                   yy::location loc) {
    if (op != AST_Binop::LOG_AND && op != AST_Binop::LOG_OR)
        internalError("Wrong short-logic operation");

    IRval lhs = evalExpr(left);

    if (lhs.getType()->type != IR_Type::DIRECT)
        semanticError(loc, "Cannon perform logical operation on non-integer type");
    auto const &ltype = dynamic_cast<IR_TypeDirect const &>(*lhs.getType());
    if (!ltype.isInteger())
        semanticError(loc, "Cannon perform logical operation on non-integer type");

    IRval res = cfg->createReg(lhs.getType());
    emitMov(res, lhs);

    IR_Block &blockLong = cfg->createBlock();
    IR_Block &blockAfter = cfg->createBlock();

    curBlock().setTerminator(IR_ExprTerminator::BRANCH, lhs);
    if (op == AST_Binop::LOG_AND) {
        cfg->linkBlocks(curBlock(), blockLong);
        cfg->linkBlocks(curBlock(), blockAfter);
    }
    else { // op == AST_Binop::LOG_OR
        cfg->linkBlocks(curBlock(), blockAfter);
        cfg->linkBlocks(curBlock(), blockLong);
    }

    selectBlock(blockLong);
    IRval rhs = evalExpr(right);

    if (!lhs.getType()->equal(*rhs.getType()))
        semanticError(loc, "Cannot do binary operation on different types");

    emitMov(res, rhs);
    curBlock().setTerminator(IR_ExprTerminator::JUMP);
    cfg->linkBlocks(curBlock(), blockAfter);

    selectBlock(blockAfter);
    return res; // Maybe PHI node here?
}

/** Get value with address of expr */
IRval IR_Generator::doAddrOf(const AST_Expr &expr) {
    if (expr.node_type == AST_PRIMARY) {
        auto const &subject = dynamic_cast<AST_Primary const &>(expr);
        if (subject.type == AST_Primary::IDENT) {
            std::optional<IRval> var = getPtrToVariable(subject.getIdent());
            if (!var.has_value())
                semanticError(subject.loc, "Unknown variable");
            return var.value();
        }
        else if (subject.type == AST_Primary::EXPR) {
            return doAddrOf(subject.getExpr());
        }
        else {
            semanticError(subject.loc, "Cannot take address from such primary expression");
        }
    }
    else if (expr.node_type == AST_UNARY_OP) { // Dereference
        auto const &subject = dynamic_cast<AST_Unop const &>(expr);
        if (subject.op != AST_Unop::DEREF)
            semanticError(subject.loc, "Cannot take address from such unary expression");
        return evalExpr(dynamic_cast<AST_Expr &>(*subject.child));
    }
    else if (expr.node_type == AST_CAST) {
        auto const &subject = dynamic_cast<AST_Cast const &>(expr);
        IRval base = doAddrOf(*subject.child);
        return emitCast(base, std::make_shared<IR_TypePtr>(getType(*subject.type_name)));
    }
    else if (expr.node_type == AST_POSTFIX) { // Index, access
        auto const &subject = dynamic_cast<AST_Postfix const &>(expr);
//        if (subject.op == AST_Postfix::INDEXATION) {
//            IRval array = evalExpr(*subject.base);
//            IRval index = evalExpr(subject.getExpr());
//
//            if (array.getType()->type != IR_Type::ARRAY)
//                semanticError("Indexation cannot be performed on non-array type");
//            auto const &arrayType = dynamic_cast<IR_TypeArray const &>(*array.getType());
//
//            IRval res = cfg->createReg(std::make_shared<IR_TypePtr>(arrayType.child));
//            curBlock().addNode(IR_Node(res, std::make_unique<IR_ExprOper>(
//                    IR_GEP, std::vector<IRval>{ array, index }))); // addOperNode
//            return res;
//        }
//        else if (subject.op == AST_Postfix::DIR_ACCESS) {
//            IRval object = evalExpr(*subject.base);
//            if (object.getType()->type != IR_Type::TSTRUCT)
//                semanticError("Element access cannot be performed on non-struct type");
//            auto structType = std::dynamic_pointer_cast<IR_TypeStruct>(object.getType());
//            auto field = structType->getField(subject.getIdent());
//            if (field == nullptr)
//                semanticError("Structure has no such field");
//
//            IRval res = cfg->createReg(std::make_shared<IR_TypePtr>(field->irType));
//            IRval index = IRval::createVal(
//                    IR_TypeDirect::type_u64,
//                    static_cast<uint64_t>(field->index));
//            curBlock().addNode(IR_Node(res, std::make_unique<IR_ExprOper>(
//                    IR_GEP, std::vector<IRval>{ object, index }))); // addOperNode
//            return res;
//        }
//        else
        if (subject.op == AST_Postfix::PTR_ACCESS) {
            IRval object = evalExpr(*subject.base);
            if (object.getType()->type != IR_Type::POINTER)
                semanticError(subject.base->loc, "Pointer access cannot be performed on non-pointer type");
            auto structPtrType = std::dynamic_pointer_cast<IR_TypePtr>(object.getType());
            if (structPtrType->child->type != IR_Type::TSTRUCT)
                semanticError(subject.base->loc, "Element access cannot be performed on non-struct type");
            auto structType = std::dynamic_pointer_cast<IR_TypeStruct>(structPtrType->child);
            auto field = structType->getField(subject.getIdent());
            if (field == nullptr)
                semanticError(subject.base->loc, "Structure has no such field");

            IRval index = IRval::createVal(IR_TypeDirect::getI32(),
                                           static_cast<int32_t>(field->index));
            auto fieldPtrType = std::make_shared<IR_TypePtr>(field->irType);
            return emitGEP(fieldPtrType, std::move(object), {
                    IRval::createVal(IR_TypeDirect::getI32(), 0), std::move(index) });
        }
        else {
            semanticError(subject.base->loc, "Cannot take address");
        }
    }
    else {
        semanticError(expr.loc, "Cannot take address");
    }
}

IRval IR_Generator::evalExpr(AST_Expr const &node) {
    switch (node.node_type) {
        case AST_COMMA_EXPR: {
            auto const &expr = static_cast<AST_CommaExpression const &>(node);
            if (expr.children.empty())
                semanticError(expr.loc, "Empty comma expression");
            for (size_t i = 0; i < expr.children.size() - 1; i++)
                evalExpr(*expr.children[i]);
            return evalExpr(*expr.children.back());
        }

        case AST_ASSIGNMENT: {
            auto const &expr = static_cast<AST_Assignment const &>(node);
            IRval rhsVal = evalExpr(*expr.rhs);
            if (expr.op != AST_Assignment::DIRECT) {
                IRval lhsVal = evalExpr(*expr.lhs);
                auto binOp = expr.toBinop();
                if (!binOp.has_value())
                    internalError("Wrong assignment type");
                rhsVal = doBinOp(binOp.value(), lhsVal, rhsVal, expr.loc);
            }
            doAssignment(*expr.lhs, rhsVal);
            return rhsVal;
        }

        case AST_TERNARY: {
            NOT_IMPLEMENTED("ternary");
        }

        case AST_BINOP: {
            auto const &expr = static_cast<AST_Binop const &>(node);
            if (isInList(expr.op, { AST_Binop::LOG_AND, AST_Binop::LOG_OR }) && isShortLogicEnabled)
                return doShortLogicOp(expr.op, *expr.lhs, *expr.rhs, expr.loc);
            return doBinOp(expr.op, evalExpr(*expr.lhs), evalExpr(*expr.rhs), expr.loc);
        }

        case AST_CAST: {
            auto const &expr = static_cast<AST_Cast const &>(node);
            IRval arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
            auto dest = getType(*expr.type_name);
            if (arg.getType()->equal(*dest))
                return arg;
            return emitCast(arg, dest);
        }

        case AST_UNARY_OP: {
            using uop = AST_Unop;

            auto const &expr = static_cast<AST_Unop const &>(node);

            if (expr.op == uop::SIZEOF_OP) {
                auto typeName = dynamic_cast<AST_TypeName *>(expr.child.get());
                if (typeName != nullptr) {
                    auto irType = getType(*typeName);
                    uint64_t bytesSize = irType->getBytesSize();
                    return IRval::createVal(IR_TypeDirect::getU64(), bytesSize);
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
                return emitOp(arg.getType(), IR_ExprOper::SUB, { zero, arg });
            }
            else if (expr.op == uop::UN_NEG) {
                IRval arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
                IRval maxv = IRval::createVal(arg.getType(), -1UL);
                return emitOp(arg.getType(), IR_ExprOper::XOR, { maxv, arg });
            }
            else if (expr.op == uop::UN_NOT) {
                IRval arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
                IRval zero = IRval::createVal(arg.getType(), 0U);
                IRval res = emitOp(IR_TypeDirect::getI1(), IR_ExprOper::EQ, { arg, zero });
                return emitCast(res, IR_TypeDirect::getI32());
            }
            else if (expr.op == uop::PRE_INC || expr.op == uop::PRE_DEC) {
                auto const &baseExpr = dynamic_cast<AST_Expr &>(*expr.child);

                IRval arg = evalExpr(baseExpr);
                IRval one = IRval::createVal(arg.getType(), 1U);
                auto op = expr.op == uop::PRE_INC ? IR_ExprOper::ADD : IR_ExprOper::SUB;
                IRval res = emitOp(arg.getType(), op, { arg, one });
                doAssignment(baseExpr, res);
                return res;
            }
            else if (expr.op == uop::DEREF) {
                IRval ptrVal = evalExpr(dynamic_cast<AST_Expr &>(*expr.child));
                if (ptrVal.getType()->type != IR_Type::POINTER)
                    semanticError(expr.loc, "Only pointer type can be dereferenced");
                auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(ptrVal.getType());
                if (ptrType->child->type == IR_Type::FUNCTION)
                    semanticError(expr.loc, "Pointer to function cannot be dereferenced");
                return emitLoad(ptrType->child, ptrVal);
            }
            else if (expr.op == uop::ADDR_OF) {
                return doAddrOf(dynamic_cast<AST_Expr const &>(*expr.child));
            }
            else {
                internalError("Unknown unary operator");
            }
        }

        case AST_POSTFIX: {
            auto const &expr = static_cast<AST_Postfix const &>(node);

            if (expr.op == AST_Postfix::CALL) {
                std::shared_ptr<IR_TypeFunc> funType;
                int dirFuncId = -1;
                std::optional<IRval> funPtr;
                if (expr.base->node_type == AST_PRIMARY) {
                    auto const &funcBase = dynamic_cast<AST_Primary const &>(*expr.base);
                    if (funcBase.type == AST_Primary::IDENT) {
                        auto funIt = functions.find(funcBase.getIdent());
                        if (funIt != functions.end()) {
                            dirFuncId = funIt->second;
                            auto const &fun = cfg->getFunction(dirFuncId);
                            funType = fun.getFuncType();
                        }
                    }
                }
                if (dirFuncId == -1) {
                    funPtr = evalExpr(*expr.base);
                    if (funPtr->getType()->type != IR_Type::POINTER)
                        semanticError(expr.base->loc, "Cannot make call with non-pointer (to functuion) type");
                    auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(funPtr->getType());
                    if (ptrType->child->type != IR_Type::FUNCTION)
                        semanticError(expr.base->loc, "Only pointers to functions can be called");
                    funType = std::dynamic_pointer_cast<IR_TypeFunc>(ptrType->child);
                }

                auto const &argsList = expr.getArgsList();

                std::vector<IRval> args;
                size_t argNum = 0;
                for (auto const &arg : argsList.children) {
                    IRval argVal = evalExpr(*arg); // TODO: promotion

                    if (argNum < funType->args.size()) {
                        if (!argVal.getType()->equal(*funType->args[argNum])) {
                            semanticError(arg->loc, "Wrong argument type");
                        }
                    }
                    else if (!funType->isVariadic) {
                        semanticError(expr.loc, "Too manny arguments in function call");
                    }

                    args.push_back(argVal);
                    argNum++;
                }
                if (argNum < funType->args.size()) {
                    semanticError(expr.loc, "Too few argument in function call");
                }

                // TODO: void return type?
                if (dirFuncId != -1)
                    return *emitCall(funType->ret, dirFuncId, args);
                else
                    return *emitIndirCall(funType->ret, *funPtr, args);
            }
            else if (expr.op == AST_Postfix::POST_INC || expr.op == AST_Postfix::POST_DEC) {
                auto const &baseExpr = dynamic_cast<AST_Expr const &>(*expr.base);

                IRval arg = evalExpr(baseExpr);
                IRval one = IRval::createVal(arg.getType(), 1UL);
                auto op = expr.op == AST_Postfix::POST_INC ? IR_ExprOper::ADD : IR_ExprOper::SUB;
                IRval res = emitOp(arg.getType(), op, { arg, one });
                doAssignment(baseExpr, res);
                return arg;
            }
            else if (expr.op == AST_Postfix::INDEXATION) {
                IRval array = evalExpr(*expr.base);
                IRval index = evalExpr(expr.getExpr());

                if (array.getType()->type == IR_Type::POINTER) {
                    auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(array.getType());
                    IRval finPtr = getPtrWithOffset(array, index);
                    return emitLoad(ptrType->child, finPtr);
                }
                else if (array.getType()->type == IR_Type::ARRAY) {
                    // At generation stage one can't get array value directly
                    // Pointers to first array's element can be eleminated at opt stage
                    internalError("Something went wrong");
                }
                else {
                    semanticError(expr.base->loc, "Indexation cannot be performed on non-array type");
                }
            }
            else if (expr.op == AST_Postfix::DIR_ACCESS) {
                IRval object = evalExpr(*expr.base);
                if (object.getType()->type != IR_Type::TSTRUCT)
                    semanticError(expr.base->loc, "Element access cannot be performed on non-struct type");
                auto structType = std::dynamic_pointer_cast<IR_TypeStruct>(object.getType());
                auto field = structType->getField(expr.getIdent());
                if (field == nullptr)
                    semanticError(expr.base->loc, "Structure has no such field");

                IRval index = IRval::createVal(IR_TypeDirect::getU64(),
                                               static_cast<uint64_t>(field->index));
                // TODO: GEP or optimize later
                return emitExtract(field->irType, std::move(object), { std::move(index) });
            }
            else if (expr.op == AST_Postfix::PTR_ACCESS) {
                IRval object = evalExpr(*expr.base);
                if (object.getType()->type != IR_Type::POINTER)
                    semanticError(expr.base->loc, "Pointer access cannot be performed on non-pointer type");
                auto structPtrType = std::dynamic_pointer_cast<IR_TypePtr>(object.getType());
                if (structPtrType->child->type != IR_Type::TSTRUCT)
                    semanticError(expr.base->loc, "Element access cannot be performed on non-struct type");
                auto structType = std::dynamic_pointer_cast<IR_TypeStruct>(structPtrType->child);
                auto field = structType->getField(expr.getIdent());
                if (field == nullptr)
                    semanticError(expr.base->loc, "Structure has no such field");

                IRval index = IRval::createVal(IR_TypeDirect::getI32(),
                                               static_cast<int32_t>(field->index));
                auto fieldPtrType = std::make_shared<IR_TypePtr>(field->irType);
                IRval elemPtr = emitGEP(fieldPtrType, std::move(object), {
                        IRval::createVal(IR_TypeDirect::getI32(), 0), std::move(index) });
                return emitLoad(field->irType, std::move(elemPtr));
            }
            else {
                internalError("Wrong postfix expression");
            }
        }

        case AST_PRIMARY: {
            auto const &expr = static_cast<AST_Primary const &>(node);

            if (expr.type == AST_Primary::CONST) {
                return getLiteralIRval(expr.getLiteral());
            }
            else if (expr.type == AST_Primary::IDENT) {
                std::optional<IRval> var = getPtrToVariable(expr.getIdent());
                if (!var.has_value()) {
                    // If there is no variables try to find pointer to function
                    auto fIt = functions.find(expr.getIdent());
                    if (fIt != functions.end()) {
                        auto fType = cfg->getFunction(fIt->second).fullType;
                        auto ptrType = std::make_shared<IR_TypePtr>(fType);
                        return IRval::createFunPtr(ptrType, fIt->second);
                    }

                    semanticError(expr.loc, "Unknown variable");
                }

                auto varType = std::dynamic_pointer_cast<IR_TypePtr>(var->getType());
                auto resType = varType->child;

                if (resType->type == IR_Type::ARRAY) {
                    // Cast pointer to array -> pointer to its first element
                    auto const &arrType = dynamic_cast<IR_TypeArray const &>(*resType);
                    auto ptrType = std::make_shared<IR_TypePtr>(arrType.child);
                    return emitCast(*var, ptrType);
                }
                else {
                    return emitLoad(resType, *var);
                }
            }
            else if (expr.type == AST_Primary::EXPR) {
                return evalExpr(expr.getExpr());
            }
            else if (expr.type == AST_Primary::STR) {
                auto [strId, fullStr] = getStringLiteral(expr.getString());
                auto it = strings.lower_bound(strId);
                if (it->first == strId)
                    return it->second;

                IRval str = IRval::createString(cfg->putString(fullStr));
                strings.emplace_hint(it, strId, str);
                return str;
            }
            else if (expr.type == AST_Primary::COMPOUND) {
                auto const &compVal = expr.getCompound();
                return getCompoundVal(getType(*compVal.compType), *compVal.val);
            }
            else {
                internalError("Wrong primary expr type");
            }
        }

        default: {
            internalError("Wrong node type in expression");
        }
    }
}

/** Create value from literal */
IRval IR_Generator::getLiteralIRval(const AST_Literal &lit) {
    auto valType = getLiteralType(lit);
    if (lit.type == INTEGER_LITERAL) {
        if (lit.isUnsigned) {
            if (lit.longCnt)
                return IRval::createVal(valType, lit.val.vu64);
            else
                return IRval::createVal(valType, lit.val.vu32);
        }
        else { // Signed
            if (lit.longCnt)
                return IRval::createVal(valType, lit.val.vi64);
            else
                return IRval::createVal(valType, lit.val.vi32);
        }
    }
    else if (lit.type == FLOAT_LITERAL) {
        if (lit.isFloat)
            return IRval::createVal(valType, lit.val.vf32);
        else
            return IRval::createVal(valType, lit.val.vf64);
    }
    else if (lit.type == CHAR_LITERAL) {
        return IRval::createVal(valType, static_cast<int8_t>(lit.val.v_char));
    }
    else {
        internalError("Wrong literal type");
    }
}

/** Get aggreagate value of given type from initializer list */
IRval IR_Generator::getCompoundVal(std::shared_ptr<IR_Type> const &type, const AST_InitializerList &lst) {
    // TODO: template and constexpr
    if (type->type == IR_Type::ARRAY) {
        auto const &arrType = dynamic_cast<IR_TypeArray const &>(*type);
        if (arrType.size < lst.children.size())
            semanticError(lst.loc, "Too many values in initializer");
    }
    else if (type->type == IR_Type::TSTRUCT) {
        auto const &structType = dynamic_cast<IR_TypeStruct const &>(*type);
        if (structType.fields.size() < lst.children.size())
            semanticError(lst.loc, "Too many values in initializer");
    }
    else {
        // TODO: Compound with single value
        NOT_IMPLEMENTED("Compound initializer for non-aggregate type");
    }

    std::vector<IRval> aggrVals;
    size_t elemNum = 0;
    for (auto const &[val, designator] : lst.children) {
        if (designator) {
            NOT_IMPLEMENTED("designators");
        }

        auto elem = evalConstantExpr(val->getExpr());
        if (!elem.has_value())
            semanticError(val->getExpr().loc, "Non-constant value in compound literal");
        if (!val->is_compound) {
            if (type->type == IR_Type::ARRAY) {
                auto const &arrType = dynamic_cast<IR_TypeArray const &>(*type);
                if (!elem->getType()->equal(*arrType.child))
                    semanticError(val->getExpr().loc, "Wrong type of initializer element");
            }
            else if (type->type == IR_Type::TSTRUCT) {
                auto const &structType = dynamic_cast<IR_TypeStruct const &>(*type);
                if (!elem->getType()->equal(*structType.fields.at(elemNum).irType))
                    semanticError(val->getExpr().loc, "Wrong type of initializer element");
            }
            else {
                internalError("Compound initializer for non-aggregate type");
            }

            aggrVals.push_back(*elem);
        }
        else {
            NOT_IMPLEMENTED("nested compound initializers");
        }

        elemNum++;
    }


    if (type->type == IR_Type::ARRAY) {
        auto const &arrType = dynamic_cast<IR_TypeArray const &>(*type);
        while (elemNum < arrType.size) {
            aggrVals.push_back(IRval::createZeroinit(arrType.child));
            elemNum++;
        }
    }
    else if (type->type == IR_Type::TSTRUCT) {
        auto const &structType = dynamic_cast<IR_TypeStruct const &>(*type);
        while (elemNum < structType.fields.size()) {
            aggrVals.push_back(IRval::createZeroinit(structType.fields.at(elemNum).irType));
            elemNum++;
        }
    }
    else {
        internalError("Compound initializer for non-aggregate type");
    }

    return IRval::createAggregate(type, std::move(aggrVals));
}

/** Get value with pointer to local or global variable */
std::optional<IRval> IR_Generator::getPtrToVariable(string_id_t ident) {
    std::vector<IRval> ptrArg;
    std::shared_ptr<IR_Type> resType;

    std::optional<IRval> var = variables.get(ident);
    if (var.has_value())
        return var.value();

    auto globalIt = globals.find(ident);
    if (globalIt != globals.end())
        return globalIt->second;

    return {};
}

std::pair<string_id_t, std::string> IR_Generator::getStringLiteral(AST_StringsSeq const &scat) {
    string_id_t parserStrId;
    std::string fullStr;
    if (scat.v.size() == 1) {
        parserStrId = scat.v[0];
        fullStr = get_string_by_id(pstate, parserStrId);
    }
    else {
        for (string_id_t sId : scat.v)
            fullStr += get_string_by_id(pstate, sId);
        parserStrId = get_string_id(pstate, fullStr.c_str(), fullStr.size());
    }
    return std::make_pair(parserStrId, fullStr);
}
