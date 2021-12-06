#include "ir_generator.hpp"

IR_Generator::IR_Generator() = default;

void IR_Generator::parseAST(const std::shared_ptr<AST_TranslationUnit>& ast) {
    for (const auto &top_instr : ast->children) {
        if (top_instr->node_type == AST_FUNC_DEF) {
            createFunction(dynamic_cast<AST_FunctionDef&>(*top_instr));
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
    IR_Function func;
    func.storage = def.specifiers->storage_specifier;
    if (func.storage == AST_DeclSpecifiers::ST_NONE)
        func.storage = AST_DeclSpecifiers::ST_EXTERN;
    func.isInline = def.specifiers->is_inline;
    func.fullType = getType(*def.specifiers, *def.decl);

    selectNewBlock();
    fillBlock(*def.body);

    fmt::print("Generation end\n");
}

int IR_Generator::blocksCounter = 0;

void IR_Generator::selectNewBlock() {
    blocks.emplace_back(blocksCounter++);
    curBlock = &blocks.back();
}

void IR_Generator::fillBlock(const AST_CompoundStmt &compStmt) {
    for (auto const &elem : compStmt.body->v) {
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
    else if (rawStmt.type == AST_Statement::COMPOUND) {
        auto const &stmt = dynamic_cast<AST_CompoundStmt const &>(rawStmt);
        if (stmt.body->v.empty())
            return;

//        IR_Block *prev = curBlock;
//        selectNewBlock();
//        prev->addNext(*curBlock); // TODO: selectNewBlockLinkedWith()
//        fillBlock(stmt);
//        // TODO: terminator
//        prev = curBlock;
//        selectNewBlock();
//        prev->addNext(*curBlock);
        NOT_IMPLEMENTED("");
    }
    else if (rawStmt.type == AST_Statement::ITER) {
//        auto const &stmt = dynamic_cast<AST_IterationStmt const &>(rawStmt);

//        if (stmt.type == AST_IterationStmt::DO_LOOP) {
//            IR_Block *prev = curBlock;
//            selectNewBlock();
//            prev->addNext(*curBlock);
//            insertStatement(*stmt.body);
//            // TODO
//        }
//        else if (stmt.type == AST_IterationStmt::WHILE_LOOP) {
//
//        }
//        else if (stmt.type == AST_IterationStmt::FOR_LOOP) {
//            NOT_IMPLEMENTED("for loop");
//        }
//        else {
//            semanticError("Wrong loop type");
//        }
        NOT_IMPLEMENTED("");
    }
    else if (rawStmt.type == AST_Statement::JUMP) {
        NOT_IMPLEMENTED("");
    }
    else if (rawStmt.type == AST_Statement::LABEL) {
        NOT_IMPLEMENTED("");
    }
    else if (rawStmt.type == AST_Statement::SELECT) {
        NOT_IMPLEMENTED("");
    }
    else {
        semanticError("Unknown statement type");
    }
}

void IR_Generator::printBlocks() const {
    for (auto const &block : blocks) {
        fmt::print("Block {}:\n", block.id);
        for (auto const &node : block.body) {
            if (node.res.has_value()) {
                std::visit([](auto const &e) { fmt::print("%{} <- ", e); }, node.res->getVal());
            }
            if (node.body->type == IR_Expr::OPERATION) {
                auto const &expr = dynamic_cast<IR_ExprOper const &>(*node.body);
                fmt::print("{} ", expr.opToString());
                for (auto const &arg : expr.args) {
                    auto pref = arg.isRegister() ? "%" : "";
                    std::visit([&pref](auto const &e) { fmt::print("{}{} ", pref, e); }, arg.getVal());
                }
                fmt::print("\n");
            }
        }
        fmt::print("\n");
    }
}
