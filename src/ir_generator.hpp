#ifndef __IR_GENERATOR_HPP__
#define __IR_GENERATOR_HPP__

#include "ir_nodes.hpp"
#include "parser/ast.hpp"
#include "utils.hpp"

#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <fmt/core.h>

[[noreturn]] inline void semanticError(const std::string &msg) {
    fmt::print(stderr, "%s\n", msg);
    throw;
}
[[noreturn]] inline void notImplemented(int line, const std::string &msg = "") {
    fmt::print(stderr, "Not implemented (%d): %s\n", line, msg);
    throw;
}
#define NOT_IMPLEMENTED(msg) notImplemented(__LINE__, __FILE__ " " msg)


[[nodiscard]] std::shared_ptr<IR_Type> getType(AST_DeclSpecifiers const &spec, AST_Declarator const &decl);
[[nodiscard]] std::shared_ptr<IR_Type> getType(AST_TypeName const &typeName);
[[nodiscard]] string_id_t getDeclaredIdent(AST_Declarator const &decl);

std::optional<IRval> evalConstantExpr(AST_Expr const &node);


struct IR_Function {
    AST_DeclSpecifiers::StorageSpec storage;
    bool isInline;
    std::shared_ptr<IR_Type> fullType;
};

class IR_Generator {
private:
    std::vector<IR_Block> blocks;
    std::vector<IR_Function> funcs;
    VariablesStack<string_id_t, IRval> variables;

    IR_Block *curBlock = nullptr;
    static int blocksCounter;
    void selectNewBlock();

    void createFunction(AST_FunctionDef const &def);
    void fillBlock(AST_CompoundStmt const &compStmt);
    void insertStatement(AST_Statement const &rawStmt);
    IRval evalExpr(AST_Expr const &node);

public:
    IR_Generator();

    void parseAST(std::shared_ptr<AST_TranslationUnit> const &ast);

    void printBlocks() const;
};

#endif /* __IR_GENERATOR_HPP__ */
