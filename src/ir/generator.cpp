#include "generator.hpp"
#include <sstream>

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


void IR_Generator::parseAST(const std::shared_ptr<AST_TranslationUnit> &ast) {
    for (const auto &top_instr: ast->children) {
        if (top_instr->node_type == AST_FUNC_DEF) {
            createFunction(dynamic_cast<AST_FunctionDef &>(*top_instr));
        }
        else if (top_instr->node_type == AST_DECLARATION) {
            auto &decl = dynamic_cast<AST_Declaration &>(*top_instr);

            // Save struct type if such is there
            if (!decl.child) {
                getPrimaryType(decl.specifiers->type_specifiers);
                continue;
            }

            int count = 0;
            for (const auto &singleDecl : decl.child->v) {
                count++;
                auto varType = getType(*decl.specifiers, *singleDecl->declarator);

                if (varType->type == IR_Type::FUNCTION) {
                    if (count > 1)
                        semanticError("Only one function van be declared per one declaration");
                    if (singleDecl->initializer)
                        semanticError("Prototypes cannot be initialized");

                    // TODO
                    auto funcIdent = getDeclaredIdent(*singleDecl->declarator);
                    auto fun = cfg->createPrototype(get_ident_by_id(funcIdent),
                                                    IR_StorageSpecifier::EXTERN, varType);
                    functions.emplace(funcIdent, fun.getId());
                    continue;
                }

                if (!singleDecl->initializer)
                    NOT_IMPLEMENTED("Unnitialized global variables");
                if (singleDecl->initializer->is_compound)
                    NOT_IMPLEMENTED("Compound initializers");

                auto ident = getDeclaredIdent(*singleDecl->declarator);
                if (globals.contains(ident))
                    semanticError("Global variable already declared");

                auto const &initExpr = dynamic_cast<AST_Expr const &>(
                        *singleDecl->initializer->val);
                auto initVal = evalConstantExpr(initExpr);
                if (!initVal.has_value())
                    semanticError("Global variable should be initialized with constant value");
                if (!initVal->getType()->equal(*varType))
                    semanticError("Cannot initialize variable with different type");

                auto ptrType = std::make_shared<IR_TypePtr>(varType);
                IRval res = cfg->createGlobal(get_ident_by_id(ident), ptrType, *initVal);
                globals.emplace(ident, res);
            }
        }
        else {
            semanticError("Only functions definitions and declarations allowed as top-level instructions");
        }
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
            semanticError("Wrong storage specifier");
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
        auto val = std::make_unique<IR_ExprAlloc>(argType, 1U);
        curBlock().addNode(IR_Node{ argPtr, std::move(val) });

        auto argVal = IRval::createFunArg(argType, curArgNum);
        auto storeNode = std::make_unique<IR_ExprOper>(
                IR_STORE, std::vector<IRval>{ argPtr, argVal });
        curBlock().addNode(IR_Node(std::move(storeNode)));

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
        if (elem->node_type == AST_DECLARATION) {
            insertDeclaration(dynamic_cast<AST_Declaration const &>(*elem));
        }
        else if (elem->node_type == AST_STATEMENT) {
            insertStatement(dynamic_cast<AST_Statement const &>(*elem));
        }
        else {
            semanticError("Only statements and declarations allowed in compound statement");
        }

        // Unreachable code doesn't even validating
        if (selectedBlock == nullptr || curBlock().termNode.has_value())
            break;
    }
    variables.decreaseLevel();
}

void IR_Generator::insertDeclaration(AST_Declaration const &decl) {
    // Save struct type if such is there
    if (!decl.child) {
        getPrimaryType(decl.specifiers->type_specifiers);
        return; // continue;
    }

    for (const auto &singleDecl : decl.child->v) {
        auto varType = getType(*decl.specifiers, *singleDecl->declarator);
        auto ident = getDeclaredIdent(*singleDecl->declarator);

        if (varType->type == IR_Type::FUNCTION) {
            semanticError("Functions are not allowed inside compound statements");
        }

        std::shared_ptr<IR_Type> ptrType = std::make_shared<IR_TypePtr>(varType);

        IRval res = cfg->createReg(ptrType);
        auto val = std::make_unique<IR_ExprAlloc>(varType, 1U);
        curBlock().addNode(IR_Node{ res, std::move(val) });

        if (variables.hasOnTop(ident))
            semanticError("Variable already declared");
        variables.put(ident, res);

        if (singleDecl->initializer) {
            if (!singleDecl->initializer->is_compound) {
                auto const &initExpr = dynamic_cast<AST_Expr const &>(
                        *singleDecl->initializer->val);
                auto initVal = evalExpr(initExpr);
                if (!initVal.getType()->equal(*varType)) {
                    semanticError("Cannot initialize variable with different type");
                }
                auto storeNode = std::make_unique<IR_ExprOper>(
                        IR_STORE, std::vector<IRval>{ res, initVal });
                curBlock().addNode(IR_Node(std::move(storeNode)));
            }
            else {
                NOT_IMPLEMENTED("Compound initializers");
            }
        }
    }
}

void IR_Generator::insertStatement(const AST_Statement &rawStmt) {
    if (rawStmt.type == AST_Statement::EXPR) {
        auto const &stmt = dynamic_cast<AST_ExprStmt const &>(rawStmt);
        if (!stmt.child)
            return;
        evalExpr(*stmt.child);
    }
    else if (rawStmt.type == AST_Statement::SELECT) {
        auto const &stmt = dynamic_cast<AST_SelectionStmt const &>(rawStmt);
        if (stmt.is_switch)
            NOT_IMPLEMENTED("switch");

        IRval cond = evalExpr(*stmt.condition);
        curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
                IR_ExprTerminator::BRANCH, cond));

        auto &blockTrue = cfg->createBlock();
        cfg->linkBlocks(curBlock(), blockTrue);

        IR_Block *blockFalse;
        if (stmt.else_body) {
            blockFalse = &cfg->createBlock();
            cfg->linkBlocks(curBlock(), *blockFalse);
        }

        auto &prevBlock = curBlock();
        IR_Block *blockAfter = nullptr;

        selectBlock(blockTrue);
        if (stmt.body->type == AST_Statement::COMPOUND)
            fillBlock(dynamic_cast<AST_CompoundStmt const &>(*stmt.body));
        else
            insertStatement(*stmt.body);

        if (selectedBlock != nullptr) { // !curBlock().termNode.has_value()
            blockAfter = &cfg->createBlock();
            cfg->linkBlocks(curBlock(), *blockAfter);
            curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
                    IR_ExprTerminator::JUMP));
        }

        if (stmt.else_body) {
            selectBlock(*blockFalse);
            if (stmt.else_body->type == AST_Statement::COMPOUND)
                fillBlock(dynamic_cast<AST_CompoundStmt const &>(*stmt.else_body));
            else
                insertStatement(*stmt.else_body);
            if (selectedBlock != nullptr) { // !curBlock().termNode.has_value()
                if (!blockAfter)
                    blockAfter = &cfg->createBlock();
                cfg->linkBlocks(curBlock(), *blockAfter);
                curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
                        IR_ExprTerminator::JUMP));
            }
        }
        else {
            if (!blockAfter)
                blockAfter = &cfg->createBlock();
            cfg->linkBlocks(prevBlock, *blockAfter);
        }

        if (blockAfter)
            selectBlock(*blockAfter);
        else
           deselectBlock();
    }
    else if (rawStmt.type == AST_Statement::ITER) {
        auto const &stmt = dynamic_cast<AST_IterationStmt const &>(rawStmt);

        auto &blockCond = cfg->createBlock();
        auto &blockLoop = cfg->createBlock();
        auto &blockAfter = cfg->createBlock();

        // Create for-loop initializer
        if (stmt.type == AST_IterationStmt::FOR_LOOP) {
            variables.increaseLevel();

            auto const &preAction = *stmt.getForLoopControls().decl;
            if (preAction.node_type == AST_STATEMENT)
                insertStatement(dynamic_cast<AST_ExprStmt const &>(preAction));
            else if (preAction.node_type == AST_DECLARATION)
                insertDeclaration(dynamic_cast<AST_Declaration const &>(preAction));
            else
                semanticError("Wrong for-loop preAction type");
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
        IRval cond = condNode ?
                evalExpr(*condNode) :
                IRval::createVal(IR_TypeDirect::type_i32, 1);
        curBlock().termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
                IR_ExprTerminator::BRANCH, cond));
        cfg->linkBlocks(curBlock(), blockLoop);
        cfg->linkBlocks(curBlock(), blockAfter);

        // Create last action
        if (stmt.type == AST_IterationStmt::FOR_LOOP) {
            auto &blockLastAct = cfg->createBlock();
            blockLastAct.termNode = IR_Node(std::make_unique<IR_ExprTerminator>(
                    IR_ExprTerminator::JUMP));
            cfg->linkBlocks(blockLastAct, blockCond);
            selectBlock(blockLastAct);
            evalExpr(*stmt.getForLoopControls().action);

            activeLoops.push({ blockLastAct.id, blockAfter.id });
        }
        else { // WHILE_LOOP, DO_LOOP
            activeLoops.push({ blockCond.id, blockAfter.id });
        }

        // Fill body
        selectBlock(blockLoop);
        if (stmt.body->type == AST_Statement::COMPOUND)
            fillBlock(dynamic_cast<AST_CompoundStmt const &>(*stmt.body));
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
    else if (rawStmt.type == AST_Statement::JUMP) {
        auto const &stmt = dynamic_cast<AST_JumpStmt const &>(rawStmt);
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
            semanticError("Unknown jump statement");
        }
    }
    else if (rawStmt.type == AST_Statement::COMPOUND) {
        auto const &stmt = dynamic_cast<AST_CompoundStmt const &>(rawStmt);
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
    else if (rawStmt.type == AST_Statement::LABEL) {
        NOT_IMPLEMENTED("Labeled statements");
    }
    else {
        semanticError("Unknown statement type");
    }
}

std::shared_ptr<ControlFlowGraph> IR_Generator::getCfg() const {
    return cfg;
}
