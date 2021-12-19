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


struct IR_FuncArgument {
    string_id_t ident;
    std::shared_ptr<IR_Type> type;
};

class IR_Generator {
private:
    VariablesStack<string_id_t, IRval> variables;
    std::map<string_id_t, int> functions; // ident -> func_id
    std::map<string_id_t, IRval> strings;
    std::map<string_id_t, uint64_t> globals; // ident -> func_id

    std::shared_ptr<ControlFlowGraph> cfg;

    IR_Block *selectedBlock = nullptr;
    IR_Block& curBlock();
    void selectBlock(IR_Block &block);
    void deselectBlock();

    std::shared_ptr<IR_TypeFunc> curFunctionType = nullptr;

    struct LoopBlocks {
        int cond, exit;
    };
    std::stack<LoopBlocks> activeLoops;

    void createFunction(AST_FunctionDef const &def);
    void fillBlock(AST_CompoundStmt const &compStmt);
    void insertStatement(AST_Statement const &rawStmt);

    bool isGeneralNumOp(AST_Binop::OpType op);
    bool isIntegerNumOp(AST_Binop::OpType op);
    bool isComparsionOp(AST_Binop::OpType op);
    IRval doConstBinOperation(AST_Binop::OpType op, IRval const &lhs, IRval const &rhs);
    std::optional<IRval> evalConstantExpr(AST_Expr const &node);
    IRval evalExpr(AST_Expr const &node);

    std::shared_ptr<IR_Type> getStructType(AST_StructOrUsionSpec const &spec);
    typedef std::vector<std::unique_ptr<AST_TypeSpecifier>> TypeSpecifiers;
    std::shared_ptr<IR_Type> getPrimaryType(TypeSpecifiers const &spec);
    template <typename DeclaratorType>
    std::shared_ptr<IR_Type> getIndirectType(DeclaratorType const *decl,
                                             std::shared_ptr<IR_Type> base);
    std::shared_ptr<IR_Type> getDirectAbstractType(AST_DirectAbstractDeclarator const *decl,
                                                   std::shared_ptr<IR_Type> base);
    std::shared_ptr<IR_Type> getDirectType(AST_DirectDeclarator const &decl,
                                           std::shared_ptr<IR_Type> base);
    std::shared_ptr<IR_Type> getType(AST_DeclSpecifiers const &spec, AST_Declarator const &decl);
    std::shared_ptr<IR_Type> getType(AST_SpecifierQualifierList const &spec, AST_Declarator const &decl);
    std::shared_ptr<IR_Type> getType(AST_TypeName const &typeName);
    string_id_t getDeclaredIdentDirect(AST_DirectDeclarator const &decl);
    string_id_t getDeclaredIdent(AST_Declarator const &decl);
    std::vector<IR_FuncArgument> getDeclaredFuncArguments(AST_Declarator const &decl);
    std::shared_ptr<IR_Type> getLiteralType(AST_Literal const &lit);

public:
    IR_Generator();

    void parseAST(std::shared_ptr<AST_TranslationUnit> const &ast);
    std::shared_ptr<ControlFlowGraph> getCfg() const;
};

#endif /* __GENERATOR_HPP__ */
