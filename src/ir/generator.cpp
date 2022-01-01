#include "generator.hpp"

IR_Generator::IR_Generator() : cfg(std::make_unique<ControlFlowGraph>()) {}

IR_Block &IR_Generator::curBlock() {
    return *selectedBlock;
}

void IR_Generator::selectBlock(IR_Block &block) {
    selectedBlock = &block;
}

void IR_Generator::deselectBlock() {
    selectedBlock = nullptr;
}

std::shared_ptr<ControlFlowGraph> IR_Generator::getCfg() const {
    return cfg;
}


void IR_Generator::parseAST(const std::shared_ptr<AST_TranslationUnit> &ast) {
    for (const auto &top_instr: ast->children) {
        if (top_instr->node_type == AST_FUNC_DEF)
            createFunction(dynamic_cast<AST_FunctionDef &>(*top_instr));
        else if (top_instr->node_type == AST_DECLARATION)
            insertGlobalDeclaration(dynamic_cast<AST_Declaration const &>(*top_instr));
        else
            internalError("Wrong top-level instruction");
    }
}

void IR_Generator::insertGlobalDeclaration(const AST_Declaration &decl) {
    // Save struct type if such is there
    if (!decl.child) {
        getPrimaryType(decl.specifiers->type_specifiers);
        return;
    }

    for (const auto &singleDecl : decl.child->v) {
        auto varType = getType(*decl.specifiers, *singleDecl->declarator);

        // Function prototype
        if (varType->type == IR_Type::FUNCTION) {
            if (decl.child->v.size() > 1)
                semanticError("Only one function can be declared per one declaration");
            if (singleDecl->initializer)
                semanticError("Prototypes cannot be initialized");

            // TODO
            auto funcIdent = getDeclaredIdent(*singleDecl->declarator);
            auto fun = cfg->createPrototype(get_ident_by_id(funcIdent),
                                            IR_StorageSpecifier::EXTERN, varType);
            functions.emplace(funcIdent, fun.getId());
            continue; // break?
        }

        if (!singleDecl->initializer)
            NOT_IMPLEMENTED("Unnitialized global variables");
        if (singleDecl->initializer->is_compound)
            NOT_IMPLEMENTED("Compound initializers");

        auto ident = getDeclaredIdent(*singleDecl->declarator);
        if (globals.contains(ident)) // TODO: check in functions (not only there)
            semanticError("Global variable already declared");

        auto initVal = evalConstantExpr(singleDecl->initializer->getExpr());
        if (!initVal.has_value())
            semanticError("Global variable should be initialized with constant value");
        if (!initVal->getType()->equal(*varType))
            semanticError("Cannot initialize variable with different type");

        auto ptrType = std::make_shared<IR_TypePtr>(varType);
        IRval res = cfg->createGlobal(get_ident_by_id(ident), ptrType, *initVal);
        globals.emplace(ident, res);
    }
}

static IR_StorageSpecifier storageSpecFromAst(AST_DeclSpecifiers::StorageSpec const &spec) {
    switch (spec) {
        case AST_DeclSpecifiers::ST_AUTO:
            return IR_StorageSpecifier::AUTO;
        case AST_DeclSpecifiers::ST_EXTERN:
            return IR_StorageSpecifier::EXTERN;
        case AST_DeclSpecifiers::ST_STATIC:
            return IR_StorageSpecifier::STATIC;
        case AST_DeclSpecifiers::ST_REGISTER:
            return IR_StorageSpecifier::REGISTER;
        default:
            internalError("Wrong storage specifier");
    }
}

void IR_Generator::createFunction(AST_FunctionDef const &def) {
    variables.increaseLevel();

    IR_StorageSpecifier storage;
    bool isInline;
    std::shared_ptr<IR_Type> fullType;

    if (def.specifiers->storage_specifier == AST_DeclSpecifiers::ST_NONE)
        storage = IR_StorageSpecifier::EXTERN;
    else
        storage = storageSpecFromAst(def.specifiers->storage_specifier);
    isInline = def.specifiers->is_inline;
    fullType = getType(*def.specifiers, *def.decl);

    auto funcIdent = getDeclaredIdent(*def.decl);
    auto fun = cfg->createFunction(get_ident_by_id(funcIdent), storage, isInline, fullType);
    functions.emplace(funcIdent, fun.getId());
    selectBlock(cfg->block(fun.getEntryBlockId()));
    curFunctionType = std::dynamic_pointer_cast<IR_TypeFunc>(fun.fullType);

    auto declArgs = getDeclaredFuncArguments(*def.decl);
    int curArgNum = 0;
    for (auto const &[argIdent, argType] : declArgs) {
        auto argPtrType = std::make_shared<IR_TypePtr>(argType);
        IRval argPtr = cfg->createReg(argPtrType);
        curBlock().addNode(IR_Node{ argPtr, std::make_unique<IR_ExprAlloc>(argType, 1U) });

        auto argVal = IRval::createFunArg(argType, curArgNum);
        curBlock().addOperNode({}, IR_ExprOper::STORE, { argPtr, argVal });

        variables.put(argIdent, argPtr);
        curArgNum++;
    }

    fillBlock(*def.body);
    curFunctionType = nullptr;
    variables.decreaseLevel();
}

void IR_Generator::fillBlock(const AST_CompoundStmt &compStmt) {
    variables.increaseLevel();
    for (auto const &elem: compStmt.body->v) {
        if (elem->node_type == AST_DECLARATION)
            insertDeclaration(dynamic_cast<AST_Declaration const &>(*elem));
        else if (elem->node_type == AST_STATEMENT)
            insertStatement(dynamic_cast<AST_Statement const &>(*elem));
        else
            internalError("Wrong node in compound statement");

        // TODO: Unreachable code doesn't even validating
        if (selectedBlock == nullptr || curBlock().termNode.has_value())
            break;
    }
    variables.decreaseLevel();
}

void IR_Generator::insertDeclaration(AST_Declaration const &decl) {
    // Save struct type if such is there
    if (!decl.child) {
        getPrimaryType(decl.specifiers->type_specifiers);
        return;
    }

    for (const auto &singleDecl : decl.child->v) {
        auto varType = getType(*decl.specifiers, *singleDecl->declarator);
        string_id_t ident = getDeclaredIdent(*singleDecl->declarator);

        if (varType->type == IR_Type::FUNCTION)
            semanticError("Functions are not allowed inside compound statements");

        std::shared_ptr<IR_Type> ptrType = std::make_shared<IR_TypePtr>(varType);

        IRval res = cfg->createReg(ptrType);
        curBlock().addNode(IR_Node{ res, std::make_unique<IR_ExprAlloc>(varType, 1U) });

        if (variables.hasOnTop(ident))
            semanticError("Variable already declared");
        variables.put(ident, res);

        if (singleDecl->initializer) {
            if (!singleDecl->initializer->is_compound) {
                IRval initVal = evalExpr(singleDecl->initializer->getExpr());
                if (!initVal.getType()->equal(*varType))
                    semanticError("Cannot initialize variable with different type");
                curBlock().addOperNode({}, IR_ExprOper::STORE, { res, initVal });
            }
            else {
                NOT_IMPLEMENTED("Compound initializers");
            }
        }
    }
}

void IR_Generator::insertStatement(const AST_Statement &rawStmt) {
    if (rawStmt.type == AST_Statement::EXPR) {
        auto const &stmt = static_cast<AST_ExprStmt const &>(rawStmt);
        if (stmt.child) // Skip empty statements
            evalExpr(*stmt.child);
    }
    else if (rawStmt.type == AST_Statement::SELECT) {
        insertIfStatement(static_cast<AST_SelectionStmt const &>(rawStmt));
    }
    else if (rawStmt.type == AST_Statement::ITER) {
        insertLoopStatement(static_cast<AST_IterationStmt const &>(rawStmt));
    }
    else if (rawStmt.type == AST_Statement::JUMP) {
        insertJumpStatement(static_cast<AST_JumpStmt const &>(rawStmt));
    }
    else if (rawStmt.type == AST_Statement::COMPOUND) {
        insertCompoindStatement(static_cast<AST_CompoundStmt const &>(rawStmt));
    }
    else if (rawStmt.type == AST_Statement::LABEL) {
        NOT_IMPLEMENTED("Labeled statements");
    }
    else {
        internalError("Wrong statement type");
    }
}

void IR_Generator::insertIfStatement(const AST_SelectionStmt &stmt) {
    if (stmt.is_switch)
        NOT_IMPLEMENTED("switch");

    IRval cond = evalExpr(*stmt.condition);
    curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
            IR_ExprTerminator::BRANCH, cond));

    IR_Block &blockTrue = cfg->createBlock();
    IR_Block *blockFalse = nullptr;
    IR_Block *blockAfter = nullptr;

    cfg->linkBlocks(curBlock(), blockTrue);
    if (stmt.else_body) {
        blockFalse = &cfg->createBlock();
        cfg->linkBlocks(curBlock(), *blockFalse);
    }
    else {
        blockAfter = &cfg->createBlock();
        cfg->linkBlocks(curBlock(), *blockAfter);
    }

    // Fill 'true' block
    selectBlock(blockTrue);
    if (stmt.body->type == AST_Statement::COMPOUND)
        fillBlock(dynamic_cast<AST_CompoundStmt const &>(*stmt.body));
    else
        insertStatement(*stmt.body);

    // Link with 'after' block, if 'true' block not terminated
    if (selectedBlock != nullptr) { // !curBlock().termNode.has_value()
        if (!blockAfter)
            blockAfter = &cfg->createBlock();
        cfg->linkBlocks(curBlock(), *blockAfter);
        curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
                IR_ExprTerminator::JUMP));
    }

    if (blockFalse) { // if (stmt.else_body)
        // Fill 'else' block
        selectBlock(*blockFalse);
        if (stmt.else_body->type == AST_Statement::COMPOUND)
            fillBlock(dynamic_cast<AST_CompoundStmt const &>(*stmt.else_body));
        else
            insertStatement(*stmt.else_body);

        // Link with 'after' block, if 'else' block not terminated
        if (selectedBlock != nullptr) { // !curBlock().termNode.has_value()
            if (!blockAfter)
                blockAfter = &cfg->createBlock();
            cfg->linkBlocks(curBlock(), *blockAfter);
            curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
                    IR_ExprTerminator::JUMP));
        }
    }

    // Select 'after' block, if such exists
    if (blockAfter)
        selectBlock(*blockAfter);
    else
        deselectBlock();
}

void IR_Generator::insertLoopStatement(const AST_IterationStmt &stmt) {
    IR_Block &blockCond = cfg->createBlock();
    IR_Block &blockLoop = cfg->createBlock();
    IR_Block &blockAfter = cfg->createBlock();

    // Create for-loop initializer
    if (stmt.type == AST_IterationStmt::FOR_LOOP) {
        variables.increaseLevel();

        // If 'for' loop hasn't initializer, it represented as empty statement
        auto const &preAction = *stmt.getForLoopControls().decl;
        if (preAction.node_type == AST_STATEMENT)
            insertStatement(dynamic_cast<AST_ExprStmt const &>(preAction));
        else if (preAction.node_type == AST_DECLARATION)
            insertDeclaration(dynamic_cast<AST_Declaration const &>(preAction));
        else
            internalError("Wrong for-loop pre action type");
    }

    // Make jump to condition
    curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
            IR_ExprTerminator::JUMP));
    if (stmt.type == AST_IterationStmt::DO_LOOP)
        cfg->linkBlocks(curBlock(), blockLoop);
    else
        cfg->linkBlocks(curBlock(), blockCond);

    // Create condition
    selectBlock(blockCond);
    auto const &condNode = stmt.getCond();
    // In 'for' loops condition can be absent
    IRval cond = condNode ? evalExpr(*condNode) : IRval::createVal(IR_TypeDirect::type_i32, 1);
    curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
            IR_ExprTerminator::BRANCH, cond));
    cfg->linkBlocks(curBlock(), blockLoop);
    cfg->linkBlocks(curBlock(), blockAfter);

    // Create last action
    if (stmt.type == AST_IterationStmt::FOR_LOOP) {
        if (stmt.getForLoopControls().action) {
            auto &blockLastAct = cfg->createBlock();
            blockLastAct.termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
                    IR_ExprTerminator::JUMP));
            cfg->linkBlocks(blockLastAct, blockCond);
            selectBlock(blockLastAct);
            evalExpr(*stmt.getForLoopControls().action);

            activeLoops.push({ blockLastAct.id, blockAfter.id });
        }
        else {
            activeLoops.push({ blockCond.id, blockAfter.id });
        }
    }
    else { // WHILE_LOOP, DO_LOOP
        activeLoops.push({ blockCond.id, blockAfter.id });
    }

    // Fill body
    selectBlock(blockLoop);
    if (stmt.body->type == AST_Statement::COMPOUND)
        fillBlock(static_cast<AST_CompoundStmt const &>(*stmt.body));
    else
        insertStatement(*stmt.body);

    // Terminate body
    if (selectedBlock != nullptr) {
        curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
                IR_ExprTerminator::JUMP));
        cfg->linkBlocks(curBlock(), cfg->block(activeLoops.top().nextIter));
    }
    selectBlock(blockAfter);

    activeLoops.pop();
    if (stmt.type == AST_IterationStmt::FOR_LOOP)
        variables.decreaseLevel();
}

void IR_Generator::insertJumpStatement(const AST_JumpStmt &stmt) {
    if (stmt.type == AST_JumpStmt::J_RET) {
        auto const &arg = stmt.getExpr();
        if (arg) {
            auto retVal = evalExpr(*arg);
            if (!curFunctionType->ret->equal(*retVal.getType()))
                semanticError("Wrong return value type");
            curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
                    IR_ExprTerminator::RET, retVal));
        }
        else {
            if (!curFunctionType->ret->equal(*IR_TypeDirect::type_void))
                semanticError("Cannot return value in void function");
            curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
                    IR_ExprTerminator::RET));
        }
        deselectBlock();
    }
    else if (stmt.type == AST_JumpStmt::J_BREAK) {
        curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
                IR_ExprTerminator::JUMP));
        cfg->linkBlocks(curBlock(), cfg->block(activeLoops.top().exit));
        deselectBlock();
    }
    else if (stmt.type == AST_JumpStmt::J_CONTINUE) {
        curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
                IR_ExprTerminator::JUMP));
        cfg->linkBlocks(curBlock(), cfg->block(activeLoops.top().nextIter));
        deselectBlock();
    }
    else if (stmt.type == AST_JumpStmt::J_GOTO) {
        NOT_IMPLEMENTED("goto");
    }
    else {
        internalError("Wrong jump statement");
    }
}

void IR_Generator::insertCompoindStatement(const AST_CompoundStmt &stmt) {
    if (stmt.body->v.empty())
        return;

    auto &compoundBlock = cfg->createBlock();
    cfg->linkBlocks(curBlock(), compoundBlock);
    curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
            IR_ExprTerminator::JUMP));
    selectBlock(compoundBlock);
    fillBlock(stmt);

    if (selectedBlock != nullptr) {
        auto &blockAfter = cfg->createBlock();
        cfg->linkBlocks(curBlock(), blockAfter);
        curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
                IR_ExprTerminator::JUMP));
        selectBlock(blockAfter);
    }
}
