#ifndef GENERATOR_HPP_INCLUDED__
#define GENERATOR_HPP_INCLUDED__

#include "nodes.hpp"
#include "cfg.hpp"
#include "parser/parser.hpp"
#include "utils.hpp"

#include <vector>
#include <map>
#include <stack>
#include <memory>
#include <optional>
#include <fmt/core.h>

class IR_Generator {
public:
    IR_Generator();
    void parse(CoreParser &parser);
    [[nodiscard]] std::shared_ptr<ControlFlowGraph> getCfg() const;

private:
    CoreParserState *pstate;

    VariablesStack<string_id_t, IRval> variables;
    std::map<string_id_t, IRval> globals;
    std::map<string_id_t, int> functions; // ident -> func_id
    std::map<string_id_t, IRval> strings;

    std::shared_ptr<ControlFlowGraph> cfg;

    IR_Block *selectedBlock = nullptr;
    IR_Block& curBlock();
    void selectBlock(IR_Block &block);
    void deselectBlock();

    std::shared_ptr<IR_TypeFunc> curFunctionType = nullptr;

    struct IR_FuncArgument {
        string_id_t ident;
        std::shared_ptr<IR_Type> type;
    };

    /** Loop or switch entry */
    class ControlStructData {
    public:
        class LoopBlocks {
        public:
            int skip, exit, before;
        };

        class SwitchBlocks {
        public:
            struct CaseBlock {
                IRval val;
                int block;
            };
            int exit;
            std::vector<CaseBlock> labels;
            std::optional<int> defaultBlock;

            void addCase(IRval val, int block);
            void setDefault(int block);
        };

        explicit ControlStructData(LoopBlocks loop);
        explicit ControlStructData(SwitchBlocks sw);

        int getExit() const;
        bool isLoop() const;
        LoopBlocks& getLoop();
        SwitchBlocks& getSwitch();

    private:
        std::variant<LoopBlocks, SwitchBlocks> data;
    };

    std::deque<ControlStructData> activeControls;
    std::optional<ControlStructData::LoopBlocks> getTopmostLoop();
    std::optional<ControlStructData::LoopBlocks> getNearestLoop();
    ControlStructData::SwitchBlocks* getNearestSwitch();

    std::map<string_id_t, int> labels;                      // label_id -> block_id
    std::vector<std::pair<int, string_id_t>> danglingGotos; // block_id -> label_id

    bool isShortLogicEnabled = true;


    std::optional<IRval> emitNode(std::optional<IRval> ret, std::unique_ptr<IR_Expr> expr);
    std::optional<IRval> emitNode(std::shared_ptr<IR_Type> ret, std::unique_ptr<IR_Expr> expr);

    IRval emitOp(std::shared_ptr<IR_Type> ret, IR_ExprOper::IR_Ops op, std::vector<IRval> args);
    void emitStore(IRval addr, IRval val);
    IRval emitLoad(std::shared_ptr<IR_Type> ret, IRval addr);
    std::optional<IRval> emitMov(IRval dst, IRval src);
    IRval emitCast(IRval srcVal, std::shared_ptr<IR_Type> dst);
    std::optional<IRval> emitCall(std::shared_ptr<IR_Type> ret, int callee, std::vector<IRval> args);
    std::optional<IRval> emitIndirCall(std::shared_ptr<IR_Type> ret, IRval callee, std::vector<IRval> args);
    IRval emitAlloc(std::shared_ptr<IR_Type> ret, std::shared_ptr<IR_Type> type, bool onHeap = false);
    IRval emitExtract(std::shared_ptr<IR_Type> ret, IRval base, std::vector<IRval> indices);
    IRval emitInsert(std::shared_ptr<IR_Type> ret, IRval base, IRval val, std::vector<IRval> indices);
    IRval emitGEP(std::shared_ptr<IR_Type> ret, IRval base, std::vector<IRval> indices);

    void createFunction(AST_FunctionDef const &def);
    void fillBlock(AST_CompoundStmt const &compStmt);
    void insertGlobalDeclaration(AST_Declaration const &decl);
    void insertDeclaration(AST_Declaration const &decl);
    IRval getInitializerVal(std::shared_ptr<IR_Type> const &type, AST_Initializer const &init);

    void insertStatement(AST_Statement const &stmt);
    void insertIfStatement(AST_SelectionStmt const &stmt);
    void insertSwitchStatement(const AST_SelectionStmt &stmt);
    void insertLoopStatement(AST_IterationStmt const &stmt);
    void insertJumpStatement(AST_JumpStmt const &stmt);
    void insertCompoundStatement(AST_CompoundStmt const &stmt);
    void insertLabeledStatement(const AST_LabeledStmt &stmt);

    static bool isGeneralNumOp(AST_Binop::OpType op);
    static bool isIntegerNumOp(AST_Binop::OpType op);
    static bool isComparsionOp(AST_Binop::OpType op);

    IRval getPtrWithOffset(IRval const &base, IRval const &index);
    void doAssignment(AST_Expr const &dest, IRval const &wrValue);
    IRval doBinOp(AST_Binop::OpType op, IRval const &lhs, IRval const &rhs);
    IRval doShortLogicOp(AST_Binop::OpType op, AST_Expr const &lhs, AST_Expr const &rhs);
    IRval doAddrOf(const AST_Expr &expr);
    IRval evalExpr(AST_Expr const &node);
    IRval getLiteralIRval(AST_Literal const &lit);
    std::optional<IRval> getPtrToVariable(string_id_t ident);
    std::pair<string_id_t, std::string> getStringLiteral(const AST_StringsSeq &scat);
    IRval getCompoundVal(std::shared_ptr<IR_Type> const &type, AST_InitializerList const &lst);

    std::optional<IRval> evalConstantExpr(AST_Expr const &node);

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
    std::shared_ptr<IR_Type> getLiteralType(AST_Literal const &lit);

    string_id_t getDeclaredIdentDirect(AST_DirectDeclarator const &decl);
    string_id_t getDeclaredIdent(AST_Declarator const &decl);
    std::vector<IR_FuncArgument> getDeclaredFuncArguments(AST_Declarator const &decl);
};

#endif /* GENERATOR_HPP_INCLUDED__ */
