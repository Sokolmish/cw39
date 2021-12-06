#include "ir_generator.hpp"

IR_Generator::IR_Generator() : blocksCounter(0) {}

IR_Block* IR_Generator::getNewBlock() {
    blocks.push_back(std::make_unique<IR_Block>(blocksCounter++));
    return blocks.back().get();
}

void IR_Generator::parseAST(const std::shared_ptr<AST_TranslationUnit> &ast) {
    for (const auto &top_instr: ast->children) {
        if (top_instr->node_type == AST_FUNC_DEF) {
            createFunction(dynamic_cast<AST_FunctionDef &>(*top_instr));
        }
        else if (top_instr->node_type == AST_DECLARATION) {
            NOT_IMPLEMENTED("");
        }
        else {
            semanticError("Only functions definitions and declarations allowed as top-level instructions");
        }
    }
}

void IR_Generator::createFunction(AST_FunctionDef const &def) {
    IR_Function fun;
    fun.storage = def.specifiers->storage_specifier;
    if (fun.storage == AST_DeclSpecifiers::ST_NONE)
        fun.storage = AST_DeclSpecifiers::ST_EXTERN;
    fun.isInline = def.specifiers->is_inline;
    fun.fullType = getType(*def.specifiers, *def.decl);

    fun.entryBlock = getNewBlock();
    curBlock = fun.entryBlock;
    fillBlock(*def.body);

    funcs.push_back(fun);
}

// TODO: move this into terminator
static void linkBlocks(IR_Block *prev, IR_Block *next) {
    prev->next.push_back(next);
    next->prev.push_back(prev);
}

void IR_Generator::fillBlock(const AST_CompoundStmt &compStmt) {
    for (auto const &elem: compStmt.body->v) {
        if (elem->node_type == AST_DECLARATION) {
            auto const &decl = dynamic_cast<AST_Declaration const &>(*elem);
            auto type = getType(*decl.specifiers, *decl.child->v[0]->declarator);
            auto ptrType = std::make_shared<IR_TypePtr>(type);
            auto ident = getDeclaredIdent(*decl.child->v[0]->declarator);

            auto res = IRval::createReg(ptrType);
            auto val = std::make_unique<IR_ExprOper>(IR_ALLOCA, std::vector<IRval>{});
            curBlock->addNode(IR_Node{ res, std::move(val) });

            variables.put(ident, res);
        }
        else if (elem->node_type == AST_STATEMENT) {
            insertStatement(dynamic_cast<AST_Statement const &>(*elem));
        }
        else {
            semanticError("Only statements and declarations allowed in compound statement");
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

        auto cond = evalExpr(*stmt.condition);
        curBlock->terminator = IR_Terminator(IR_Terminator::BRANCH, cond);

        auto blockTrue = getNewBlock();
        linkBlocks(curBlock, blockTrue);

        IR_Block *blockFalse;
        if (stmt.else_body) {
            blockFalse = getNewBlock();
            linkBlocks(curBlock, blockFalse);
        }

        auto prevBlock = curBlock;
        auto blockAfter = getNewBlock();

        curBlock = blockTrue;
        insertStatement(*stmt.body);
        linkBlocks(curBlock, blockAfter);
        curBlock->terminator = IR_Terminator(IR_Terminator::JUMP);

        if (stmt.else_body) {
            curBlock = blockFalse;
            insertStatement(*stmt.else_body);
            linkBlocks(curBlock, blockAfter);
            curBlock->terminator = IR_Terminator(IR_Terminator::JUMP);
        }
        else {
            linkBlocks(prevBlock, blockAfter);
        }

        curBlock = blockAfter;
    }
    else if (rawStmt.type == AST_Statement::ITER) {
        auto const &stmt = dynamic_cast<AST_IterationStmt const &>(rawStmt);
        if (stmt.type != AST_IterationStmt::WHILE_LOOP)
            NOT_IMPLEMENTED("Only while loops implemented");

        auto blockCond =  getNewBlock();
        auto blockLoop =  getNewBlock();
        auto blockAfter = getNewBlock();

        curBlock->terminator = IR_Terminator(IR_Terminator::JUMP);
        linkBlocks(curBlock, blockCond);
        curBlock = blockCond;

        auto cond = evalExpr(*std::get<std::unique_ptr<AST_Expr>>(stmt.control));
        curBlock->terminator = IR_Terminator(IR_Terminator::BRANCH, cond);
        linkBlocks(curBlock, blockLoop);
        linkBlocks(curBlock, blockAfter);

        activeLoops.push({ blockCond, blockAfter });
        curBlock = blockLoop;
        insertStatement(*stmt.body);
        activeLoops.pop();

        curBlock->terminator = IR_Terminator(IR_Terminator::JUMP);
        linkBlocks(curBlock, blockCond);

        curBlock = blockAfter;
    }
    else if (rawStmt.type == AST_Statement::JUMP) {
        auto const &stmt = dynamic_cast<AST_JumpStmt const &>(rawStmt);
        if (stmt.type == AST_JumpStmt::J_RET) {
            auto const &arg = std::get<std::unique_ptr<AST_Expr>>(stmt.arg);
            if (arg) {
                auto retVal = evalExpr(*arg);
                curBlock->terminator = IR_Terminator(IR_Terminator::RET, retVal);
            }
            else {
                curBlock->terminator = IR_Terminator(IR_Terminator::RET);
            }
        }
        else if (stmt.type == AST_JumpStmt::J_BREAK) {
            curBlock->terminator = IR_Terminator(IR_Terminator::JUMP);
            linkBlocks(curBlock, activeLoops.top().exit);
        }
        else if (stmt.type == AST_JumpStmt::J_CONTINUE) {
            curBlock->terminator = IR_Terminator(IR_Terminator::BRANCH);
            linkBlocks(curBlock, activeLoops.top().cond);
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

        auto compoundBlock = getNewBlock();
        linkBlocks(curBlock, compoundBlock);
        curBlock->terminator = IR_Terminator(IR_Terminator::JUMP);
        curBlock = compoundBlock;
        fillBlock(stmt);

        auto blockAfter = getNewBlock();
        linkBlocks(curBlock, blockAfter);
        curBlock->terminator = IR_Terminator(IR_Terminator::JUMP);
        curBlock = blockAfter;
    }
    else if (rawStmt.type == AST_Statement::LABEL) {
        NOT_IMPLEMENTED("Labeled statements");
    }
    else {
        semanticError("Unknown statement type");
    }
}

static void printBlock(IR_Block const &block) {
    fmt::print("Block {}:\n", block.id);
    for (auto const &node: block.body) {
        if (node.res.has_value())
            fmt::print("{} <- ", node.res->to_string());
        if (node.body->type == IR_Expr::OPERATION) {
            auto const &expr = dynamic_cast<IR_ExprOper const &>(*node.body);
            fmt::print("{} ", expr.opToString());
            for (auto const &arg: expr.args)
                fmt::print("{} ", arg.to_string());
            fmt::print("\n");
        }
    }

    if (block.terminator.type == IR_Terminator::NONE) {
        fmt::print("; No terminator\n");
    }
    else if (block.terminator.type == IR_Terminator::RET) {
        if (!block.terminator.val.has_value())
            fmt::print("ret\n");
        else
            fmt::print("ret {}\n", block.terminator.val->to_string());
    }
    else if (block.terminator.type == IR_Terminator::JUMP) {
        fmt::print("jump ->{}\n", block.next[0]->id);
    }
    else if (block.terminator.type == IR_Terminator::BRANCH) {
        fmt::print("branch {} ->{} ->{}\n", block.terminator.val->to_string(),
                   block.next[0]->id, block.next[1]->id);
    }
    else {
        fmt::print("; Unknown terminator\n");
    }

    fmt::print("\n");
}

void IR_Generator::printBlocks() const {
    for (auto const &block : blocks) {
        printBlock(*block);
    }
}
