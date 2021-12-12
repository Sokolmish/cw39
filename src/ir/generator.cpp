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
            NOT_IMPLEMENTED("Top level declarations");
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
    ControlFlowGraph::Function fun;
    if (def.specifiers->storage_specifier == AST_DeclSpecifiers::ST_NONE)
        fun.storage = IR_StorageSpecifier::EXTERN;
    else
        fun.storage = storageSpecFromAst(def.specifiers->storage_specifier);
    fun.isInline = def.specifiers->is_inline;
    fun.fullType = getType(*def.specifiers, *def.decl);

    selectBlock(cfg->createFunction(fun));
    fillBlock(*def.body);
}

void IR_Generator::fillBlock(const AST_CompoundStmt &compStmt) {
    for (auto const &elem: compStmt.body->v) {
        if (elem->node_type == AST_DECLARATION) {
            auto const &decl = dynamic_cast<AST_Declaration const &>(*elem);
            auto type = getType(*decl.specifiers, *decl.child->v[0]->declarator);
            auto ptrType = std::make_shared<IR_TypePtr>(type);
            auto ident = getDeclaredIdent(*decl.child->v[0]->declarator);

            auto res = cfg->createReg(ptrType);
            auto val = std::make_unique<IR_ExprAlloc>(type, 1U);
            curBlock().addNode(IR_Node{ res, std::move(val) });

            variables.put(ident, res);
        }
        else if (elem->node_type == AST_STATEMENT) {
            insertStatement(dynamic_cast<AST_Statement const &>(*elem));
        }
        else {
            semanticError("Only statements and declarations allowed in compound statement");
        }

        // Unreachable code doesn't even validating
        if (selectedBlock == nullptr || curBlock().terminator.exist())
            break;
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

        auto cond = evalExpr(*stmt.condition);
        curBlock().terminator = IR_Terminator(IR_Terminator::BRANCH, cond);

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

        if (!curBlock().terminator.exist()) {
            blockAfter = &cfg->createBlock();
            cfg->linkBlocks(curBlock(), *blockAfter);
            curBlock().terminator = IR_Terminator(IR_Terminator::JUMP);
        }

        if (stmt.else_body) {
            selectBlock(*blockFalse);
            if (stmt.else_body->type == AST_Statement::COMPOUND)
                fillBlock(dynamic_cast<AST_CompoundStmt const &>(*stmt.else_body));
            else
                insertStatement(*stmt.else_body);
            if (!curBlock().terminator.exist()) {
                if (!blockAfter)
                    blockAfter = &cfg->createBlock();
                cfg->linkBlocks(curBlock(), *blockAfter);
                curBlock().terminator = IR_Terminator(IR_Terminator::JUMP);
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
        if (stmt.type != AST_IterationStmt::WHILE_LOOP)
            NOT_IMPLEMENTED("Only while loops implemented");

        auto &blockCond = cfg->createBlock();
        auto &blockLoop = cfg->createBlock();
        auto &blockAfter = cfg->createBlock();

        curBlock().terminator = IR_Terminator(IR_Terminator::JUMP);
        cfg->linkBlocks(curBlock(), blockCond);
        selectBlock(blockCond);

        auto cond = evalExpr(*std::get<std::unique_ptr<AST_Expr>>(stmt.control));
        curBlock().terminator = IR_Terminator(IR_Terminator::BRANCH, cond);
        cfg->linkBlocks(curBlock(), blockLoop);
        cfg->linkBlocks(curBlock(), blockAfter);

        activeLoops.push({ blockCond.id, blockAfter.id });
        selectBlock(blockLoop);
        if (stmt.body->type == AST_Statement::COMPOUND)
            fillBlock(dynamic_cast<AST_CompoundStmt const &>(*stmt.body));
        else
            insertStatement(*stmt.body);
        activeLoops.pop();

        if (selectedBlock != nullptr) {
            curBlock().terminator = IR_Terminator(IR_Terminator::JUMP);
            cfg->linkBlocks(curBlock(), blockCond);
        }
        selectBlock(blockAfter);
    }
    else if (rawStmt.type == AST_Statement::JUMP) {
        auto const &stmt = dynamic_cast<AST_JumpStmt const &>(rawStmt);
        if (stmt.type == AST_JumpStmt::J_RET) {
            auto const &arg = std::get<std::unique_ptr<AST_Expr>>(stmt.arg);
            if (arg) {
                auto retVal = evalExpr(*arg);
                curBlock().terminator = IR_Terminator(IR_Terminator::RET, retVal);
            }
            else {
                curBlock().terminator = IR_Terminator(IR_Terminator::RET);
            }
        }
        else if (stmt.type == AST_JumpStmt::J_BREAK) {
            curBlock().terminator = IR_Terminator(IR_Terminator::JUMP);
            cfg->linkBlocks(curBlock(), cfg->block(activeLoops.top().exit));
        }
        else if (stmt.type == AST_JumpStmt::J_CONTINUE) {
            curBlock().terminator = IR_Terminator(IR_Terminator::JUMP);
            cfg->linkBlocks(curBlock(), cfg->block(activeLoops.top().cond));
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
        curBlock().terminator = IR_Terminator(IR_Terminator::JUMP);
        selectBlock(compoundBlock);
        fillBlock(stmt);

        if (selectedBlock != nullptr) {
            auto &blockAfter = cfg->createBlock();
            cfg->linkBlocks(curBlock(), blockAfter);
            curBlock().terminator = IR_Terminator(IR_Terminator::JUMP);
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

std::shared_ptr<ControlFlowGraph> const &IR_Generator::getCfg() const {
    return cfg;
}
