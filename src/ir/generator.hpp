#ifndef GENERATOR_HPP_INCLUDED__
#define GENERATOR_HPP_INCLUDED__

#include "nodes.hpp"
#include "unit.hpp"
#include "parser/parsing_context.hpp"
#include "parser/core_driver.hpp"
#include "utils.hpp"

#include <vector>
#include <map>
#include <stack>
#include <memory>
#include <optional>
#include <list>
#include <fmt/core.h>


template <typename K, typename V>
class VariablesStack {
public:
    VariablesStack() {
        increaseLevel();
    }

    void increaseLevel() {
        data.push_back(std::map<K, V>());
    }

    void decreaseLevel() {
        if (data.empty())
            throw std::logic_error("Attemt to decrease level of empty VariablesStack");
        data.pop_back();
    }

    std::optional<V> get(K const &key) const {
        for (auto lay = data.rbegin(); lay != data.rend(); ++lay) {
            auto it = lay->find(key);
            if (it != lay->end())
                return it->second;
        }
        return {};
    }

    void put(K const &key, V const &val) {
        data.back().emplace(key, val);
    }

    bool hasOnTop(K const &key) {
        return data.back().contains(key);
    }

private:
    std::vector<std::map<K, V>> data;
};


class IR_Generator {
public:
    IR_Generator(AbstractSyntaxTree const &ast, ParsingContext &ctx);

    std::shared_ptr<IntermediateUnit> getIR() const;

private:
    ParsingContext &ctx;

    VariablesStack<string_id_t, IRval> variables;
    std::map<string_id_t, IRval> globals;
    std::map<string_id_t, int> functions; // ident -> func_id
    std::map<string_id_t, IRval> strings;

    std::shared_ptr<IntermediateUnit> iunit;

    IR_Block *selectedBlock = nullptr;
    IR_Block& curBlock();
    void selectBlock(IR_Block &block);
    void deselectBlock();

    IntermediateUnit::Function *curFunc;

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
    std::vector<std::tuple<int, string_id_t, yy::location>> danglingGotos; // block_id, label_id, loc

    bool isShortLogicEnabled = true;

    struct IntrinsicSig {
        IR_ExprOper::IR_Ops op;
        std::shared_ptr<IR_Type> retType;
        std::vector<std::shared_ptr<IR_Type>> argsTypes;
    };

    std::map<ParsingContext::ReservedWords, IntrinsicSig> intrinsicSignatures;
    void setupIntrinsicSignatures();


    // Common

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

    // Control generator

    void genTransUnit(AST_TranslationUnit const &tunit);

    void checkNewGlobalName(string_id_t id, yy::location loc);
    void createFunction(AST_FunctionDef const &def);
    void fillBlock(AST_CompoundStmt const &compStmt);
    void insertGlobalDeclaration(AST_Declaration const &decl);
    void insertDeclaration(AST_Declaration const &decl);
    IRval getInitializerVal(std::shared_ptr<IR_Type> const &type, AST_Initializer const &init);

    void insertStatement(AST_Stmt const &stmt);
    void insertIfStatement(AST_SelectionStmt const &stmt);
    void insertSwitchStatement(const AST_SelectionStmt &stmt);
    void insertLoopStatement(AST_IterStmt const &stmt);
    void insertJumpStatement(AST_JumpStmt const &stmt);
    void insertCompoundStatement(AST_CompoundStmt const &stmt);
    void insertLabeledStatement(const AST_LabeledStmt &stmt);

    static constexpr bool isGeneralNumOp(AST_Binop::OpType op);
    static constexpr bool isIntegerNumOp(AST_Binop::OpType op);
    static constexpr bool isComparsionOp(AST_Binop::OpType op);

    // Expresions generator

    IRval evalExpr(AST_Expr const &node);
    std::optional<IRval> evalConstantExpr(AST_Expr const &node);

    IRval evalCommaExpr(AST_CommaExpression const &expr);
    IRval evalAssignmentExpr(AST_Assignment const &expr);
    IRval evalBinopExpr(AST_Binop const &expr);
    IRval evalCastExpr(AST_Cast const &expr);
    IRval evalUnopExpr(AST_Unop const &expr);
    IRval evalPostfixExpr(AST_Postfix const &expr);
    IRval evalPrimaryExpr(AST_Primary const &expr);

    IRval getPtrWithOffset(IRval const &base, IRval const &index, bool positive);
    void doAssignment(AST_Expr const &dest, IRval const &wrValue);
    IRval doBinOp(AST_Binop::OpType op, IRval const &lhs, IRval const &rhs, yy::location loc);
    IRval doShortLogicOp(AST_Binop::OpType op, AST_Expr const &left, AST_Expr const &right, yy::location loc);
    IRval doAddrOf(AST_Expr const &expr);
    IRval doCall(AST_Postfix const &expr);
    IRval doIntrinsic(string_id_t intrIdent, AST_Postfix const &expr);

    IRval getCompoundVal(std::shared_ptr<IR_Type> const &type, AST_InitializerList const &lst);
    IRval getLiteralIRval(AST_Literal const &lit);
    std::optional<IRval> getPtrToVariable(string_id_t ident);
    std::pair<string_id_t, std::string> getStringLiteral(const AST_StringsSeq &scat);

    // Types generator

    std::shared_ptr<IR_Type> getStructType(AST_UStructSpec const &spec);
    using TypeSpecifiers = std::vector<AST_TypeSpecifier*>;
    std::shared_ptr<IR_Type> getPrimaryType(TypeSpecifiers const &spec);
    template <typename DeclaratorType>
    std::shared_ptr<IR_Type> getIndirectType(DeclaratorType const *decl, std::shared_ptr<IR_Type> base);
    std::shared_ptr<IR_Type> getDirAbstrType(AST_DirAbstrDeclarator const *decl, std::shared_ptr<IR_Type> base);
    std::shared_ptr<IR_Type> getDirType(AST_DirDeclarator const &decl, std::shared_ptr<IR_Type> base);
    std::shared_ptr<IR_Type> getType(AST_DeclSpecifiers const &spec, AST_Declarator const &decl);
    std::shared_ptr<IR_Type> getType(AST_SpecsQualsList const &spec, AST_Declarator const &decl);
    std::shared_ptr<IR_Type> getType(AST_TypeName const &typeName);
    std::shared_ptr<IR_Type> getLiteralType(AST_Literal const &lit);

    bool checkVoidParam(std::vector<AST_ParameterDeclaration*> const &params);
    std::vector<IR_FuncArgument> getDeclaredFuncArgs(AST_Declarator const &decl);

    IntermediateUnit::FunLinkage getFunLinkage(AST_DeclSpecifiers::StorageSpec spec, yy::location const &loc);
    IntermediateUnit::VarLinkage getGVarLinkage(AST_DeclSpecifiers::StorageSpec spec, yy::location const &loc);

    // Errors

    [[noreturn]] void semanticError(yy::location loc, std::string msg);

    class semantic_exception : public cw39_exception {
    public:
        semantic_exception(ParsingContext &ctx, yy::location const &loc, std::string msg);
    };
};

#endif /* GENERATOR_HPP_INCLUDED__ */
