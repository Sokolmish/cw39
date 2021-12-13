#ifndef __GENERATOR_HPP__
#define __GENERATOR_HPP__

#include "nodes.hpp"
#include "cfg.hpp"
#include "parser/ast.hpp"
#include "utils.hpp"

#include <vector>
#include <map>
#include <stack>
#include <memory>
#include <optional>
#include <fmt/core.h>


[[nodiscard]] std::shared_ptr<IR_Type> getType(AST_DeclSpecifiers const &spec, AST_Declarator const &decl);
[[nodiscard]] std::shared_ptr<IR_Type> getType(AST_TypeName const &typeName);
[[nodiscard]] string_id_t getDeclaredIdent(AST_Declarator const &decl);
[[nodiscard]] std::shared_ptr<IR_Type> getLiteralType(AST_Literal const &lit);

std::optional<IRval> evalConstantExpr(AST_Expr const &node);

class IR_Generator {
private:
    VariablesStack<string_id_t, IRval> variables;

    std::shared_ptr<ControlFlowGraph> cfg;

    IR_Block *selectedBlock = nullptr;
    IR_Block& curBlock();
    void selectBlock(IR_Block &block);
    void deselectBlock();

    struct LoopBlocks {
        int cond, exit;
    };
    std::stack<LoopBlocks> activeLoops;

    void createFunction(AST_FunctionDef const &def);
    void fillBlock(AST_CompoundStmt const &compStmt);
    void insertStatement(AST_Statement const &rawStmt);
    IRval evalExpr(AST_Expr const &node);

public:
    IR_Generator();

    void parseAST(std::shared_ptr<AST_TranslationUnit> const &ast);
    [[nodiscard]] std::shared_ptr<ControlFlowGraph> const& getCfg() const;
};

#endif /* __GENERATOR_HPP__ */
