#include "generator.hpp"

constexpr bool IR_Generator::isGeneralNumOp(AST_Binop::OpType op) {
    using b = AST_Binop;
    return isInList(op, b::ADD, b::SUB, b::MUL, b::DIV);
}

constexpr bool IR_Generator::isIntegerNumOp(AST_Binop::OpType op) {
    using b = AST_Binop;
    return isInList(op, b::SHL, b::SHR, b::REM, b::BIT_AND, b::BIT_OR, b::BIT_XOR);
}

constexpr bool IR_Generator::isComparsionOp(AST_Binop::OpType op) {
    using b = AST_Binop;
    return isInList(op, b::LT, b::GT, b::LE, b::GE, b::EQ, b::NE, b::LOG_AND, b::LOG_OR);
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
IRval IR_Generator::getPtrWithOffset(IRval const &base, IRval const &index, bool positive) {
    auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(base.getType());

    // TODO: use GEP

    IRval fixedIndex = index;
    if (!fixedIndex.getType()->equal(*IR_TypeDirect::getU64()))
        fixedIndex = emitCast(index, IR_TypeDirect::getU64());

    IRval scaledIndex = fixedIndex;
    if (ptrType->child->getBytesSize() != 1) {
        uint64_t bytesSize = static_cast<uint64_t>(ptrType->child->getBytesSize());
        IRval mult = IRval::createVal(IR_TypeDirect::getU64(), bytesSize);
        scaledIndex = emitOp(fixedIndex.getType(), IR_ExprOper::MUL, { fixedIndex, mult });
    }

    IRval iptr = emitCast(base, IR_TypeDirect::getU64());
    auto finOp = positive ? IR_ExprOper::ADD : IR_ExprOper::SUB;
    IRval wOffset = emitOp(IR_TypeDirect::getU64(), finOp, { iptr, scaledIndex });
    return emitCast(wOffset, ptrType);
}

/** Store wrValue in object described by dest (variable, pointer, field, etc) */
void IR_Generator::doAssignment(AST_Expr const &dest, IRval const &wrValue) {
    if (dest.node_type == AST_PRIMARY) { // Identifiers
        auto &assignee = static_cast<AST_Primary const &>(dest);
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

        IRval valToStore = wrValue;
        if (!destVarPtrType->child->equal(*valToStore.getType())) {
            // semanticError(assignee.loc, "Cannot assign values of different types");
            valToStore = emitCast(std::move(valToStore), destVarPtrType->child);
        }
        emitStore(*destVar, std::move(valToStore));
    }
    else if (dest.node_type == AST_UNARY_OP) { // Dereference write
        auto &assignee = static_cast<AST_Unop const &>(dest);
        if (assignee.op != AST_Unop::DEREF)
            semanticError(assignee.loc, "Cannot be assigned");

        IRval ptrVal = evalExpr(dynamic_cast<AST_Expr &>(*assignee.child));
        if (ptrVal.getType()->type != IR_Type::POINTER)
            semanticError(assignee.loc, "Only pointer type can be dereferenced");
        auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(ptrVal.getType());
        if (ptrType->child->type == IR_Type::FUNCTION)
            semanticError(assignee.loc, "Pointer to function cannot be dereferenced");

        IRval valToStore = wrValue;
        if (!ptrType->child->equal(*valToStore.getType())) {
            // semanticError(assignee.loc, "Cannot assign values of different types");
            valToStore = emitCast(std::move(valToStore), ptrType->child);
        }
        emitStore(ptrVal, std::move(valToStore));
    }
    else if (dest.node_type == AST_POSTFIX) { // Accessors
        auto &assignee = static_cast<AST_Postfix const &>(dest);
        if (assignee.op == AST_Postfix::DIR_ACCESS) {
            IRval base = evalExpr(*assignee.base);
            if (base.getType()->type != IR_Type::TSTRUCT)
                semanticError(assignee.loc, "Element access cannot be performed on non-struct type");
            auto &structType = dynamic_cast<IR_TypeStruct const &>(*base.getType());
            auto field = structType.getField(assignee.getIdent());
            if (field == nullptr)
                semanticError(assignee.loc, "Structure has no such field");

            IRval index = IRval::createVal(IR_TypeDirect::getU64(),
                                           static_cast<uint64_t>(field->index));

            IRval valToStore = wrValue;
            if (!field->irType->equal(*valToStore.getType())) {
                // semanticError(assignee.loc, "Cannot assign values of different types");
                valToStore = emitCast(std::move(valToStore), field->irType);
            }
            IRval res = emitInsert(base.getType(), base, std::move(valToStore), { index }); // TODO: GEP

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

            // TODO: types check?

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
                throw cw39_internal_error("Something went wrong");
            }
            else if (base.getType()->type == IR_Type::POINTER) {
                auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(base.getType());
                IRval finPtr = getPtrWithOffset(base, index, true);

                IRval valToStore = wrValue;
                if (!ptrType->child->equal(*valToStore.getType())) {
                    // semanticError(assignee.loc, "Cannot assign values of different types");
                    valToStore = emitCast(std::move(valToStore), ptrType->child);
                }
                emitStore(finPtr, std::move(valToStore));
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

/** Create node with specified binary operation on numbers */
IRval IR_Generator::doBinOp(AST_Binop::OpType op, IRval const &lhs, IRval const &rhs, yy::location loc) {
    // Do pointers arithmetics
    if (lhs.getType()->type == IR_Type::POINTER || rhs.getType()->type == IR_Type::POINTER) {
        if (!isInList(op, AST_Binop::ADD, AST_Binop::SUB))
            semanticError(loc, "Pointes arithmetics allows only addictions or subtractions");
        if (lhs.getType()->type == rhs.getType()->type)
            semanticError(loc, "Pointes arithmetics cannot be applied between two pointers");

        if (rhs.getType()->type == IR_Type::POINTER) {
            if (op == AST_Binop::SUB)
                semanticError(loc, "Canon subtract pointer from number");
            return doBinOp(op, rhs, lhs, loc);
        }
        // Now lhs is pointer

        if (lhs.getType()->getBytesSize() == 0)
            semanticError(loc, "Pointes arithmetics cannot be applied to void pointers");

        if (op == AST_Binop::ADD)
            return getPtrWithOffset(lhs, rhs, true);
        else // op == AST_Binop::SUB
            return getPtrWithOffset(lhs, rhs, false);
    }

    if (lhs.getType()->type != IR_Type::DIRECT)
        semanticError(loc, "Wrong arithmetic type");

    // Cast different types
    if (!lhs.getType()->equal(*rhs.getType())) {
        auto cmnType = IR_TypeDirect::getCommonDirType(lhs.getType(), rhs.getType());
        IRval cmnLhs = emitCast(lhs, cmnType);
        IRval cmnRhs = emitCast(rhs, cmnType);
        return doBinOp(op, cmnLhs, cmnRhs, loc);
    }

    auto const &ltype = dynamic_cast<IR_TypeDirect const &>(*lhs.getType());

    if (isGeneralNumOp(op)) {
        switch (op) {
            case AST_Binop::ADD:
                return emitOp(lhs.getType(), IR_ExprOper::ADD, { lhs, rhs });
            case AST_Binop::SUB:
                return emitOp(lhs.getType(), IR_ExprOper::SUB, { lhs, rhs });
            case AST_Binop::MUL:
                return emitOp(lhs.getType(), IR_ExprOper::MUL, { lhs, rhs });
            case AST_Binop::DIV:
                return emitOp(lhs.getType(), IR_ExprOper::DIV, { lhs, rhs });
            default:
                throw cw39_internal_error("Wrong general arithmetic operation");
        }
    }
    else if (isIntegerNumOp(op)) {
        if (!ltype.isInteger())
            semanticError(loc, "Operation cannot be applied to non-integer types");

        switch (op) {
            case AST_Binop::REM:
                return emitOp(lhs.getType(), IR_ExprOper::REM, { lhs, rhs });
            case AST_Binop::SHL:
                return emitOp(lhs.getType(), IR_ExprOper::SHL, { lhs, rhs });
            case AST_Binop::SHR:
                return emitOp(lhs.getType(), IR_ExprOper::SHR, { lhs, rhs });
            case AST_Binop::BIT_XOR:
                return emitOp(lhs.getType(), IR_ExprOper::XOR, { lhs, rhs });
            case AST_Binop::BIT_AND:
                return emitOp(lhs.getType(), IR_ExprOper::AND, { lhs, rhs });
            case AST_Binop::BIT_OR:
                return emitOp(lhs.getType(), IR_ExprOper::OR, { lhs, rhs });
            default:
                throw cw39_internal_error("Wrong general arithmetic operation");
        }
    }
    else if (isComparsionOp(op)) {
        switch (op) {
            case AST_Binop::EQ:
                return emitOp(IR_TypeDirect::getI1(), IR_ExprOper::EQ, { lhs, rhs });
            case AST_Binop::NE:
                return emitOp(IR_TypeDirect::getI1(), IR_ExprOper::NE, { lhs, rhs });
            case AST_Binop::GT:
                return emitOp(IR_TypeDirect::getI1(), IR_ExprOper::GT, { lhs, rhs });
            case AST_Binop::LT:
                return emitOp(IR_TypeDirect::getI1(), IR_ExprOper::LT, { lhs, rhs });
            case AST_Binop::GE:
                return emitOp(IR_TypeDirect::getI1(), IR_ExprOper::GE, { lhs, rhs });
            case AST_Binop::LE:
                return emitOp(IR_TypeDirect::getI1(), IR_ExprOper::LE, { lhs, rhs });
            case AST_Binop::LOG_AND:
                return emitOp(IR_TypeDirect::getI1(), IR_ExprOper::LAND, { lhs, rhs });
            case AST_Binop::LOG_OR:
                return emitOp(IR_TypeDirect::getI1(), IR_ExprOper::LOR, { lhs, rhs });
            default:
                throw cw39_internal_error("Wrong comparsion operation");
        }
    }
    else {
        throw cw39_internal_error("Wrong binary operation");
    }
}

/** Create nodes and blocks for logical operation with short evaluation */
IRval IR_Generator::doShortLogicOp(AST_Binop::OpType op, AST_Expr const &left, AST_Expr const &right,
                                   yy::location loc) {
    if (op != AST_Binop::LOG_AND && op != AST_Binop::LOG_OR)
        throw cw39_internal_error("Wrong short-logic operation");

    IRval lhs = evalExpr(left);

    if (lhs.getType()->type != IR_Type::DIRECT)
        semanticError(loc, "Cannon perform logical operation on non-integer type");
    auto &ltype = dynamic_cast<IR_TypeDirect const &>(*lhs.getType());
    if (!ltype.isInteger())
        semanticError(loc, "Cannon perform logical operation on non-integer type");

    IRval res = iunit->createReg(lhs.getType());
    emitMov(res, lhs);

    IR_Block &blockLong = curFunc->cfg.createBlock();
    IR_Block &blockAfter = curFunc->cfg.createBlock();

    curBlock().setTerminator(IR_ExprTerminator::BRANCH, lhs);
    if (op == AST_Binop::LOG_AND) {
        curFunc->cfg.linkBlocks(curBlock(), blockLong);
        curFunc->cfg.linkBlocks(curBlock(), blockAfter);
    }
    else { // op == AST_Binop::LOG_OR
        curFunc->cfg.linkBlocks(curBlock(), blockAfter);
        curFunc->cfg.linkBlocks(curBlock(), blockLong);
    }

    selectBlock(blockLong);
    IRval rhs = evalExpr(right);

    if (!lhs.getType()->equal(*rhs.getType()))
        semanticError(loc, "Cannot do binary operation on different types");

    emitMov(res, rhs);
    curBlock().setTerminator(IR_ExprTerminator::JUMP);
    curFunc->cfg.linkBlocks(curBlock(), blockAfter);

    selectBlock(blockAfter);
    return res; // Maybe PHI node here?
}

/** Get value with address of expr */
IRval IR_Generator::doAddrOf(const AST_Expr &expr) {
    if (expr.node_type == AST_PRIMARY) {
        auto &subject = dynamic_cast<AST_Primary const &>(expr);
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
        auto &subject = dynamic_cast<AST_Unop const &>(expr);
        if (subject.op != AST_Unop::DEREF)
            semanticError(subject.loc, "Cannot take address from such unary expression");
        return evalExpr(dynamic_cast<AST_Expr &>(*subject.child));
    }
    else if (expr.node_type == AST_CAST) {
        auto &subject = dynamic_cast<AST_Cast const &>(expr);
        IRval base = doAddrOf(*subject.child);
        return emitCast(base, std::make_shared<IR_TypePtr>(getType(*subject.type_name)));
    }
    else if (expr.node_type == AST_POSTFIX) { // Index, access
        auto &subject = dynamic_cast<AST_Postfix const &>(expr);
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

IRval IR_Generator::doCall(AST_Postfix const &expr) {
    std::shared_ptr<IR_TypeFunc> funType;
    int dirFuncId = -1;
    std::optional<IRval> funPtr;
    if (expr.base->node_type == AST_PRIMARY) {
        auto &funcBase = dynamic_cast<AST_Primary const &>(*expr.base);
        if (funcBase.type == AST_Primary::IDENT) {
            auto funIt = functions.find(funcBase.getIdent());
            if (funIt != functions.end()) {
                dirFuncId = funIt->second;
                auto const &fun = iunit->getFunction(dirFuncId);
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

    auto &argsList = expr.getArgsList();

    std::vector<IRval> args;
    size_t argNum = 0;
    for (auto const &arg : argsList.children) {
        IRval argVal = evalExpr(*arg);

        if (argNum < funType->args.size()) {
            auto &argType = funType->args[argNum];
            if (!argVal.getType()->equal(*argType)) {
                if (argType->type == IR_Type::DIRECT && argVal.getType()->type == IR_Type::DIRECT) {
                    auto cmnType = IR_TypeDirect::getCommonDirType(argType, argVal.getType());
                    argVal = emitCast(std::move(argVal), cmnType);
                }
                else {
                    semanticError(arg->loc, "Wrong argument type");
                }
            }
        }
        else if (funType->isVariadic) { // Default g promotion
            if (argVal.getType()->type == IR_Type::DIRECT) {
                auto &dirType = dynamic_cast<IR_TypeDirect const &>(*argVal.getType());
                if (dirType.spec == IR_TypeDirect::F32)
                    argVal = emitCast(std::move(argVal), IR_TypeDirect::getF64());
            }
        }
        else {
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


IRval IR_Generator::evalExpr(AST_Expr const &node) {
    switch (node.node_type) {
        case AST_COMMA_EXPR:
            return evalCommaExpr(static_cast<AST_CommaExpression const &>(node));

        case AST_ASSIGNMENT:
            return evalAssignmentExpr(static_cast<AST_Assignment const &>(node));

        case AST_TERNARY:
            throw cw39_not_implemented("Ternary operator");

        case AST_BINOP:
            return evalBinopExpr(static_cast<AST_Binop const &>(node));

        case AST_CAST:
            return evalCastExpr(static_cast<AST_Cast const &>(node));

        case AST_UNARY_OP:
            return evalUnopExpr(static_cast<AST_Unop const &>(node));

        case AST_POSTFIX:
            return evalPostfixExpr(static_cast<AST_Postfix const &>(node));

        case AST_PRIMARY:
            return evalPrimaryExpr(static_cast<AST_Primary const &>(node));

        default:
            throw cw39_internal_error("Wrong node type in expression");
    }
}

IRval IR_Generator::evalCommaExpr(AST_CommaExpression const &expr) {
    if (expr.children.empty())
        semanticError(expr.loc, "Empty comma expression");
    for (size_t i = 0; i < expr.children.size() - 1; i++)
        evalExpr(*expr.children[i]);
    return evalExpr(*expr.children.back());
}

IRval IR_Generator::evalAssignmentExpr(AST_Assignment const &expr) {
    IRval rhsVal = evalExpr(*expr.rhs);
    if (expr.op != AST_Assignment::DIRECT) {
        IRval lhsVal = evalExpr(*expr.lhs);
        auto binOp = expr.toBinop();
        if (!binOp.has_value())
            throw std::logic_error("Wrong assignment type");
        rhsVal = doBinOp(binOp.value(), lhsVal, rhsVal, expr.loc);
    }
    doAssignment(*expr.lhs, rhsVal);
    return rhsVal;
}

IRval IR_Generator::evalBinopExpr(AST_Binop const &expr) {
    bool isLogic = isInList(expr.op, AST_Binop::LOG_AND, AST_Binop::LOG_OR);
    if (isLogic && isShortLogicEnabled)
        return doShortLogicOp(expr.op, *expr.lhs, *expr.rhs, expr.loc);
    return doBinOp(expr.op, evalExpr(*expr.lhs), evalExpr(*expr.rhs), expr.loc);
}

IRval IR_Generator::evalCastExpr(AST_Cast const &expr) {
    IRval arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
    auto dest = getType(*expr.type_name);
    if (arg.getType()->equal(*dest))
        return arg;
    return emitCast(arg, dest);
}

IRval IR_Generator::evalUnopExpr(AST_Unop const &expr) {
    switch (expr.op) {
        case AST_Unop::SIZEOF_OP: {
            auto typeName = dynamic_cast<AST_TypeName *>(expr.child);
            if (typeName != nullptr) {
                auto irType = getType(*typeName);
                uint64_t bytesSize = irType->getBytesSize();
                return IRval::createVal(IR_TypeDirect::getU64(), bytesSize);
            }
            else {
                throw cw39_not_implemented("Sizeof expression");
            }
        }

        case AST_Unop::UN_PLUS: {
            return evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
        }

        case AST_Unop::UN_MINUS: {
            IRval arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
            IRval zero = IRval::createVal(arg.getType(), 0U);
            return emitOp(arg.getType(), IR_ExprOper::SUB, { zero, arg });
        }

        case AST_Unop::UN_NEG: {
            IRval arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
            IRval maxv = IRval::createVal(arg.getType(), -1UL);
            return emitOp(arg.getType(), IR_ExprOper::XOR, { maxv, arg });
        }

        case AST_Unop::UN_NOT: {
            IRval arg = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
            IRval zero = IRval::createVal(arg.getType(), 0U);
            IRval res = emitOp(IR_TypeDirect::getI1(), IR_ExprOper::EQ, { arg, zero });
            return emitCast(res, IR_TypeDirect::getI32());
        }

        case AST_Unop::PRE_INC:
        case AST_Unop::PRE_DEC: {
            auto &baseExpr = dynamic_cast<AST_Expr const &>(*expr.child);
            auto op = expr.op == AST_Unop::PRE_INC ? IR_ExprOper::ADD : IR_ExprOper::SUB;

            IRval arg = evalExpr(baseExpr);
            IRval one = IRval::createVal(arg.getType(), 1U);
            IRval res = emitOp(arg.getType(), op, { arg, one });
            doAssignment(baseExpr, res);
            return res;
        }

        case AST_Unop::DEREF: {
            IRval ptrVal = evalExpr(dynamic_cast<AST_Expr const &>(*expr.child));
            if (ptrVal.getType()->type != IR_Type::POINTER)
                semanticError(expr.loc, "Only pointer type can be dereferenced");
            auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(ptrVal.getType());
            if (ptrType->child->type == IR_Type::FUNCTION)
                semanticError(expr.loc, "Pointer to function cannot be dereferenced");
            return emitLoad(ptrType->child, ptrVal);
        }

        case AST_Unop::ADDR_OF: {
            return doAddrOf(dynamic_cast<AST_Expr const &>(*expr.child));
        }

        default: {
            throw std::logic_error("Wrong unary operator");
        }
    }
}

IRval IR_Generator::evalPostfixExpr(AST_Postfix const &expr) {
    switch (expr.op) {
        case AST_Postfix::CALL: {
            return doCall(expr);
        }

        case AST_Postfix::POST_INC:
        case AST_Postfix::POST_DEC: {
            auto &baseExpr = dynamic_cast<AST_Expr const &>(*expr.base);
            auto op = expr.op == AST_Postfix::POST_INC ? IR_ExprOper::ADD : IR_ExprOper::SUB;

            IRval arg = evalExpr(baseExpr);
            IRval one = IRval::createVal(arg.getType(), 1UL);
            IRval res = emitOp(arg.getType(), op, { arg, one });
            doAssignment(baseExpr, res);
            return arg;
        }

        case AST_Postfix::INDEXATION: {
            IRval array = evalExpr(*expr.base);
            IRval index = evalExpr(expr.getExpr());

            if (array.getType()->type == IR_Type::POINTER) {
                auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(array.getType());
                IRval finPtr = getPtrWithOffset(array, index, true);
                return emitLoad(ptrType->child, finPtr);
            }
            else if (array.getType()->type == IR_Type::ARRAY) {
                // At generation stage one can't get array value directly
                // Pointers to first array's element can be eleminated at opt stage
                throw cw39_internal_error("Something went wrong");
            }
            else {
                semanticError(expr.base->loc, "Indexation cannot be performed on non-array type");
            }
        }

        case AST_Postfix::DIR_ACCESS: {
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

        case AST_Postfix::PTR_ACCESS: {
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

        default: {
            throw std::logic_error("Wrong postfix expression");
        }
    }
}

IRval IR_Generator::evalPrimaryExpr(AST_Primary const &expr) {
    switch (expr.type) {
        case AST_Primary::CONST: {
            return getLiteralIRval(expr.getLiteral());
        }

        case AST_Primary::IDENT: {
            // TODO: check for __func__

            std::optional<IRval> var = getPtrToVariable(expr.getIdent());
            if (!var.has_value()) {
                // If there is no variables try to find pointer to function
                auto fIt = functions.find(expr.getIdent());
                if (fIt != functions.end()) {
                    auto fType = iunit->getFunction(fIt->second).fullType;
                    auto ptrType = std::make_shared<IR_TypePtr>(fType);
                    return IRval::createFunPtr(ptrType, fIt->second);
                }

                semanticError(expr.loc, "Unknown variable");
            }

            auto varType = std::dynamic_pointer_cast<IR_TypePtr>(var->getType());
            auto resType = varType->child;

            if (resType->type == IR_Type::ARRAY) {
                // Cast pointer to array -> pointer to its first element
                auto &arrType = dynamic_cast<IR_TypeArray const &>(*resType);
                auto ptrType = std::make_shared<IR_TypePtr>(arrType.child);
                return emitCast(*var, ptrType);
            }
            else {
                return emitLoad(resType, *var);
            }
        }

        case AST_Primary::EXPR: {
            return evalExpr(expr.getExpr());
        }

        case AST_Primary::STR: {
            auto [strId, fullStr] = getStringLiteral(expr.getString());
            auto it = strings.lower_bound(strId);
            if (it->first == strId)
                return it->second;

            IRval str = IRval::createString(iunit->putString(fullStr));
            strings.emplace_hint(it, strId, str);
            return str;
        }

        case AST_Primary::COMPOUND: {
            auto &compVal = expr.getCompound();
            return getCompoundVal(getType(*compVal.compType), *compVal.val);
        }

        default: {
            throw std::logic_error("Wrong primary expr type");
        }
    }
}


/** Create value from literal */
IRval IR_Generator::getLiteralIRval(const AST_Literal &lit) {
    auto valType = getLiteralType(lit);
    switch (lit.type) {
        case AST_Literal::INTEGER: {
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

        case AST_Literal::FLOAT: {
            if (lit.isFloat)
                return IRval::createVal(valType, lit.val.vf32);
            else
                return IRval::createVal(valType, lit.val.vf64);
        }

        case AST_Literal::CHARACTER: {
            return IRval::createVal(valType, static_cast<int8_t>(lit.val.v_char));
        }

        default: {
            throw std::logic_error("Wrong literal type");
        }
    }
}

/** Get aggreagate value of given type from initializer list */
IRval IR_Generator::getCompoundVal(std::shared_ptr<IR_Type> const &type, const AST_InitializerList &lst) {
    if (type->type == IR_Type::ARRAY) {
        auto &arrType = dynamic_cast<IR_TypeArray const &>(*type);
        if (arrType.size < lst.children.size())
            semanticError(lst.loc, "Too many values in initializer");
    }
    else if (type->type == IR_Type::TSTRUCT) {
        auto &structType = dynamic_cast<IR_TypeStruct const &>(*type);
        if (structType.fields.size() < lst.children.size())
            semanticError(lst.loc, "Too many values in initializer");
    }
    else {
        // TODO: Compound with single value
        throw cw39_not_implemented("Compound initializer for non-aggregate type");
    }

    std::vector<IRval> aggrVals;
    size_t elemNum = 0;
    for (auto const &[val, designator] : lst.children) {
        if (designator) {
            throw cw39_not_implemented("Designators");
        }

        auto elemOpt = evalConstantExpr(val->getExpr());
        if (!elemOpt.has_value())
            semanticError(val->getExpr().loc, "Non-constant value in compound literal");
        IRval elemVal = std::move(*elemOpt);

        if (!val->is_compound) {
            if (type->type == IR_Type::ARRAY) {
                auto &arrType = dynamic_cast<IR_TypeArray const &>(*type);
                if (!elemVal.getType()->equal(*arrType.child)) {
                    // semanticError(val->getExpr().loc, "Wrong type of initializer element");
                    elemVal = emitCast(std::move(elemVal), arrType.child);
                }
            }
            else if (type->type == IR_Type::TSTRUCT) {
                auto &structType = dynamic_cast<IR_TypeStruct const &>(*type);
                auto &fieldType = structType.fields.at(elemNum).irType;
                if (!elemVal.getType()->equal(*fieldType)) {
                    // semanticError(val->getExpr().loc, "Wrong type of initializer element");
                    elemVal = emitCast(std::move(elemVal), fieldType);
                }
            }
            else {
                throw cw39_internal_error("Compound initializer for non-aggregate type");
            }

            aggrVals.emplace_back(std::move(elemVal));
        }
        else {
            throw cw39_not_implemented("nested compound initializers");
        }

        elemNum++;
    }

    if (type->type == IR_Type::ARRAY) {
        auto &arrType = dynamic_cast<IR_TypeArray const &>(*type);
        while (elemNum < arrType.size) {
            aggrVals.push_back(IRval::createZeroinit(arrType.child));
            elemNum++;
        }
    }
    else if (type->type == IR_Type::TSTRUCT) {
        auto &structType = dynamic_cast<IR_TypeStruct const &>(*type);
        while (elemNum < structType.fields.size()) {
            aggrVals.push_back(IRval::createZeroinit(structType.fields.at(elemNum).irType));
            elemNum++;
        }
    }
    else {
        throw cw39_internal_error("Compound initializer for non-aggregate type");
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
        fullStr = ctx.getStringById(parserStrId);
    }
    else {
        for (string_id_t sId : scat.v)
            fullStr += ctx.getStringById(sId);
        parserStrId = ctx.getStringId(fullStr);
    }
    return std::make_pair(parserStrId, fullStr);
}
