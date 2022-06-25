#ifndef PARSER_AST_H_INCLUDED__
#define PARSER_AST_H_INCLUDED__

#include <cstdint>
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include "common.hpp"
#include "print_tree.hpp"
#include "yy_location.hpp"
#include "parsing_context.hpp"

using ast_enum_t = int;

struct AST_Node {
    yy::location loc;

    AST_Node() = default;
    virtual ~AST_Node() = default;

    AST_Node(AST_Node const &) = delete;
    AST_Node& operator=(AST_Node const &) = delete;

    [[nodiscard]] virtual TreeNodeRef getTreeNode(ParsingContext const &pctx) const = 0;
};

struct AST_UStructSpec;
struct AST_EnumSpecifier;
struct AST_Declarator;
struct AST_AbstrDeclarator;
struct AST_TypeName;
struct AST_InitializerList;
struct AST_Initializer;
struct AST_ParameterTypeList;
struct AST_Stmt;

// =================================================
//                    Expressions
// =================================================

struct AST_Expr : public AST_Node {};

struct AST_StringsSeq final : public AST_Node {
    std::vector<string_id_t> v;

    AST_StringsSeq() = default;
    AST_StringsSeq* append(string_id_t str);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_Primary final : public AST_Expr {
    struct CompoundLiteral {
        AST_TypeName *compType = nullptr;
        AST_InitializerList *val = nullptr;
    };

    enum PrimType : ast_enum_t { IDENT, EXPR, STR, CONST, COMPOUND } type;
    std::variant<
        AST_Expr*, AST_StringsSeq*, string_id_t, AST_Literal, CompoundLiteral
    > v;

    explicit AST_Primary(PrimType type);

    string_id_t getIdent() const;
    AST_StringsSeq const& getString() const;
    AST_Literal getLiteral() const;
    AST_Expr const& getExpr() const;
    CompoundLiteral const& getCompound() const;

    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_ArgumentsList final : public AST_Node {
    std::vector<AST_Expr*> children;

    AST_ArgumentsList() = default;
    AST_ArgumentsList* append(AST_Expr *arg);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_Postfix final : public AST_Expr {
    enum OpType : ast_enum_t {
        CALL, POST_INC, POST_DEC, INDEXATION, DIR_ACCESS, PTR_ACCESS
    } op;
    AST_Expr *base = nullptr;
    std::variant<AST_Node*, string_id_t> arg; // Expr ArgumentsList

    explicit AST_Postfix(OpType type);

    AST_Expr const& getExpr() const; // TODO: getArgExpr
    AST_ArgumentsList const& getArgsList() const;
    string_id_t getIdent() const;

    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_Unop final : public AST_Expr {
    enum OpType : ast_enum_t {
        PRE_INC, PRE_DEC,
        ADDR_OF, DEREF, UN_PLUS, UN_MINUS, UN_NEG, UN_NOT,
        SIZEOF_OP,
    } op;
    AST_Node *child; // Expr TypeName

    AST_Unop(OpType op, AST_Expr *child);
    AST_Unop(OpType op, AST_TypeName *child);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_Cast final : public AST_Expr {
    AST_TypeName *type_name;
    AST_Expr *child;

    AST_Cast(AST_TypeName *type, AST_Expr *child);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_Binop final : public AST_Expr {
    static constexpr const char * const opstr[] = {
            "*", "/", "%", "+", "-", "<<", ">>",
            "<", ">", "<=", ">=", "==", "!=",
            "&", "^", "|", "&&", "||"
    };

    enum OpType : ast_enum_t {
        ADD, SUB, MUL, DIV, REM, SHL, SHR,
        BIT_AND, BIT_XOR, BIT_OR,
        LOG_AND, LOG_OR,
        LT, GT, LE, GE, EQ, NE,
    } op;
    AST_Expr *lhs, *rhs;

    AST_Binop(OpType op, AST_Expr *lhs, AST_Expr *rhs);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_Ternary final : public AST_Expr {
    AST_Expr *cond, *v_true, *v_false;

    AST_Ternary(AST_Expr *cond, AST_Expr *vt, AST_Expr *vf);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_Assignment final : public AST_Expr {
    static constexpr const char * const opstr[] = {
            "=", "*=", "/=", "%=", "+=", "-=", "<<=", ">>=",
            "&=", "^=", "|="
    };

    enum OpType : ast_enum_t {
        DIRECT, MUL, DIV, REM, ADD, SUB, SHL, SHR, AND, XOR, OR
    } op;
    AST_Expr *lhs, *rhs;

    std::optional<AST_Binop::OpType> toBinop() const;

    AST_Assignment(OpType op, AST_Expr *lhs, AST_Expr *rhs);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_CommaExpression final : public AST_Expr {
    std::vector<AST_Expr*> children;

    AST_CommaExpression(AST_Expr *expr1, AST_Expr *expr2);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};


// =================================================
//                   Specifiers
// =================================================

struct AST_TypeQuals final : public AST_Node {
    enum QualType : ast_enum_t { Q_CONST, Q_RESTRICT, Q_VOLATILE };
    bool is_const, is_restrict, is_volatile;

    AST_TypeQuals();
    explicit AST_TypeQuals(QualType init_qual);
    AST_TypeQuals* update(QualType new_qual);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_TypeSpecifier final : public AST_Node {
    static constexpr const char * const typestr[] = {
            "void", "char", "short", "int", "long",
            "signed", "unsigned", "float", "double",
            "<unistruct>", "<enum>", "<<named>>"
    };

    enum TypeSpec : ast_enum_t {
        T_VOID, T_CHAR, T_SHORT, T_INT, T_LONG,
        T_SIGNED, T_UNSIGNED, T_FLOAT, T_DOUBLE,
        T_UNISTRUCT, T_ENUM, T_NAMED,
    } spec_type;
    AST_Node *v; // AST_StructOrUsionSpec AST_TypeName AST_EnumSpecifier

    explicit AST_TypeSpecifier(TypeSpec type);
    explicit AST_TypeSpecifier(AST_UStructSpec *spec);
    explicit AST_TypeSpecifier(AST_TypeName *spec);
    explicit AST_TypeSpecifier(AST_EnumSpecifier *spec);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_DeclSpecifiers final : public AST_Node {
    static constexpr const char * const storstr[] = {
            "<storage>", "extern", "static", "auto",
            "register", "typedef", "weak",
    };

    enum StorageSpec : ast_enum_t {
        ST_NONE, ST_EXTERN, ST_STATIC, ST_AUTO,
        ST_REGISTER, ST_TYPEDEF, ST_WEAK,
    };
    enum FuncQual : ast_enum_t { Q_INLINE, Q_PURE, Q_FCONST };

    StorageSpec storage_specifier = ST_NONE;
    std::vector<AST_TypeSpecifier*> type_specifiers;
    AST_TypeQuals *type_qualifiers;
    bool is_inline = false;
    bool is_pure = false;
    bool is_fconst = false;

    explicit AST_DeclSpecifiers(AST_TypeQuals *quals);
    AST_DeclSpecifiers* update_storage(ast_enum_t val);
    AST_DeclSpecifiers* update_type_spec(AST_TypeSpecifier *val);
    AST_DeclSpecifiers* update_type_qual(ast_enum_t val);
    AST_DeclSpecifiers* update_func_qual(ast_enum_t val);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_SpecsQualsList final : public AST_Node {
    std::vector<AST_TypeSpecifier*> type_specifiers;
    AST_TypeQuals *type_qualifiers = nullptr;

    AST_SpecsQualsList(std::vector<AST_TypeSpecifier*> specs, AST_TypeQuals *quals);
    AST_SpecsQualsList(AST_TypeQuals::QualType qual, AST_TypeQuals *quals);
    AST_SpecsQualsList(AST_TypeSpecifier* type, AST_TypeQuals *quals);
    AST_SpecsQualsList* append_qual(AST_TypeQuals::QualType qual);
    AST_SpecsQualsList* append_spec(AST_TypeSpecifier* type);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_StructDeclarator final : public AST_Node {
    AST_Declarator *declarator;
    AST_Expr *bitwidth;

    AST_StructDeclarator(AST_Declarator *decl, AST_Expr *width);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_StructDeclaratorList final : public AST_Node {
    std::vector<AST_StructDeclarator*> children;

    explicit AST_StructDeclaratorList(AST_StructDeclarator *init);
    AST_StructDeclaratorList* append(AST_StructDeclarator *decl);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_StructDeclaration final : public AST_Node {
    AST_SpecsQualsList *type;
    AST_StructDeclaratorList *child;

    AST_StructDeclaration(AST_SpecsQualsList *type, AST_StructDeclaratorList *child);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_StructDeclarationList final : public AST_Node {
    std::vector<AST_StructDeclaration*> children;

    explicit AST_StructDeclarationList(AST_StructDeclaration *init);
    AST_StructDeclarationList* append(AST_StructDeclaration *decl);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_UStructSpec final : public AST_Node {
    bool is_union;
    string_id_t name;
    AST_StructDeclarationList *body;

    AST_UStructSpec(bool is_union, string_id_t name, AST_StructDeclarationList *body);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_Enumerator final : public AST_Node {
    string_id_t name;
    AST_Expr *val;

    AST_Enumerator(string_id_t name, AST_Expr *val);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_EnumeratorList final : public AST_Node {
    std::vector<AST_Enumerator*> v;

    explicit AST_EnumeratorList(AST_Enumerator *init);
    AST_EnumeratorList* append(AST_Enumerator *enumer);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_EnumSpecifier final : public AST_Node {
    string_id_t name;
    AST_EnumeratorList *body;

    AST_EnumSpecifier(string_id_t name, AST_EnumeratorList *body);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};


// =================================================
//                 Declarations
// =================================================

struct AST_InitDeclarator final : public AST_Node {
    AST_Declarator *declarator;
    AST_Initializer *initializer;

    AST_InitDeclarator(AST_Declarator *decl, AST_Initializer *init);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_InitDeclaratorList final : public AST_Node {
    std::vector<AST_InitDeclarator*> v;

    explicit AST_InitDeclaratorList(AST_InitDeclarator *init);
    AST_InitDeclaratorList* append(AST_InitDeclarator *decl);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_Declaration final : public AST_Node {
    AST_DeclSpecifiers *specifiers;
    AST_InitDeclaratorList *child;

    AST_Declaration(AST_DeclSpecifiers *spec, AST_InitDeclaratorList *child);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_DirDeclarator final : public AST_Node {
    enum DeclType : ast_enum_t { NAME, NESTED, ARRAY, FUNC } type;
    std::variant<string_id_t, AST_Node*> base; // DirectDeclarator Declarator

    AST_TypeQuals *arr_type_qual = nullptr;
    AST_Expr *arr_size = nullptr;
    AST_ParameterTypeList *func_args = nullptr;
    bool isStatic = false;

    explicit AST_DirDeclarator(DeclType dtype);

    string_id_t getIdent() const;
    AST_DirDeclarator const& getBaseDirectDecl() const;
    AST_Declarator const& getBaseDecl() const;

    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_Pointer final : public AST_Node {
    AST_TypeQuals *qualifiers;
    AST_Pointer *child;

    AST_Pointer(AST_TypeQuals *qual, AST_Pointer *child);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_Declarator final : public AST_Node {
    AST_DirDeclarator *direct;
    AST_Pointer *ptr;

    AST_Declarator(AST_DirDeclarator *decl, AST_Pointer *ptr);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_ParameterDeclaration final : public AST_Node {
    AST_DeclSpecifiers *specifiers;
    AST_Declarator *child; // Declarator

    AST_ParameterDeclaration(AST_DeclSpecifiers *spec, AST_Declarator *child);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_ParameterList final : public AST_Node {
    std::vector<AST_ParameterDeclaration*> v;

    explicit AST_ParameterList(AST_ParameterDeclaration *init);
    AST_ParameterList* append(AST_ParameterDeclaration *decl);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_ParameterTypeList final : public AST_Node {
    AST_ParameterList *v;
    bool has_ellipsis;

    AST_ParameterTypeList(AST_ParameterList *child, bool ellipsis);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_TypeName final : public AST_Node {
    AST_SpecsQualsList *qual;
    std::optional<std::variant<AST_AbstrDeclarator*, AST_Declarator*>> declarator;

    using optDeclType = decltype(declarator);

    AST_TypeName(AST_SpecsQualsList *qual, AST_AbstrDeclarator *decl);
    AST_TypeName(AST_SpecsQualsList *qual, AST_Declarator *decl);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_DirAbstrDeclarator final : public AST_Node {
    enum DeclType : ast_enum_t { NESTED, ARRAY, FUNC } type;
    AST_Node *base = nullptr;

    AST_Expr *arr_size = nullptr;
    AST_ParameterTypeList *func_args = nullptr;

    explicit AST_DirAbstrDeclarator(DeclType dtype);

    AST_DirAbstrDeclarator const& getBaseDirectDecl() const;
    AST_AbstrDeclarator const& getBaseDecl() const;

    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_AbstrDeclarator final : public AST_Node {
    AST_DirAbstrDeclarator *direct;
    AST_Pointer *ptr;

    AST_AbstrDeclarator(AST_DirAbstrDeclarator *decl, AST_Pointer *pointer);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};


// =================================================
//                 Initializers
// =================================================

struct AST_Designator final : public AST_Node {
    std::variant<AST_Expr*, string_id_t> val;
    bool is_index; // indexation or field name

    explicit AST_Designator(AST_Expr *val);
    explicit AST_Designator(string_id_t field);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_InitializerList final : public AST_Node {
    struct AST_InitializerListElem {
        AST_Initializer *val;
        AST_Designator *designator;

        AST_InitializerListElem(AST_Initializer *v, AST_Designator *desig);
        [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const;
    };

    std::vector<AST_InitializerListElem> children;

    AST_InitializerList(AST_Initializer *init_v, AST_Designator *init_desig);
    AST_InitializerList* append(AST_Initializer *val, AST_Designator *desig);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_Initializer final : public AST_Node {
    bool is_compound;
    AST_Node *val; // AST_InitializerList AST_Expr

    AST_Expr const& getExpr() const;
    AST_InitializerList const& getInitList() const;

    explicit AST_Initializer(AST_InitializerList *nest);
    explicit AST_Initializer(AST_Expr *val);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};


// =================================================
//                   Statements
// =================================================

struct AST_Stmt : public AST_Node {
    AST_Stmt() = default;
};

struct AST_LabeledStmt final : public AST_Stmt {
    std::variant<AST_Expr*, string_id_t> label;
    AST_Stmt *child;
    enum LabelType : ast_enum_t { SIMPL, SW_CASE, SW_DEFAULT } type;

    AST_LabeledStmt(AST_Expr *label, AST_Stmt *stmt, LabelType type);
    AST_LabeledStmt(string_id_t label, AST_Stmt *stmt, LabelType type);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;

    string_id_t getIdent() const;
    AST_Expr const& getExpr() const;
};

struct AST_BlockItemList final : public AST_Node {
    std::vector<AST_Node*> v; // Declaration Statement

    AST_BlockItemList();
    AST_BlockItemList* append(AST_Node *child);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_CompoundStmt final : public AST_Stmt {
    AST_BlockItemList *body;

    explicit AST_CompoundStmt(AST_BlockItemList *body);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_ExprStmt final : public AST_Stmt {
    AST_Expr *child;

    explicit AST_ExprStmt(AST_Expr *child);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_SelectionStmt final : public AST_Stmt {
    bool is_switch;
    AST_Expr *condition = nullptr;
    AST_Stmt *body = nullptr;
    AST_Stmt *else_body = nullptr;

    explicit AST_SelectionStmt(bool sw);

    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_IterStmt final : public AST_Stmt {
    struct ForLoopControls {
        AST_Node *decl;
        AST_ExprStmt *cond;
        AST_Expr *action;

        [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const;
    };

    enum LoopType : ast_enum_t { WHILE_LOOP, DO_LOOP, FOR_LOOP } type;
    AST_Stmt *body;
    std::variant<AST_Expr*, ForLoopControls> control;

    AST_IterStmt(LoopType ltype, AST_Stmt *body);

    AST_Expr* getCond() const;
    ForLoopControls const& getForLoopControls() const;

    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_JumpStmt final : public AST_Stmt {
    enum JumpType : ast_enum_t {
        J_GOTO, J_CONTINUE, J_BREAK, J_RET
    } type;
    std::variant<AST_Expr*, string_id_t> arg;

    explicit AST_JumpStmt(JumpType jtype);
    AST_JumpStmt(JumpType jtype, AST_Expr *arg);
    AST_JumpStmt(JumpType jtype, string_id_t arg);

    AST_Expr* getExpr() const;
    string_id_t getIdent() const;

    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};


// =================================================
//                Top-level elements
// =================================================

struct AST_FunctionDef final : public AST_Node {
    AST_DeclSpecifiers *specifiers;
    AST_Declarator *decl;
    AST_CompoundStmt *body;

    AST_FunctionDef(AST_DeclSpecifiers *spec, AST_Declarator *decl, AST_CompoundStmt *body);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};

struct AST_TranslationUnit final : public AST_Node {
    std::vector<AST_Node*> children;

    AST_TranslationUnit();
    AST_TranslationUnit* append(AST_Node *node);
    [[nodiscard]] TreeNodeRef getTreeNode(ParsingContext const &pctx) const override;
};


class AbstractSyntaxTree {
public:
    using loc_t = yy::location;

    AbstractSyntaxTree() = default;

    AST_TranslationUnit *top = nullptr;

    // Expressions

    AST_Primary* mkPrimIdent(string_id_t id, loc_t loc);
    AST_Primary* mkPrimExpr(AST_Expr *expr, loc_t loc);
    AST_Primary* mkPrimStr(AST_StringsSeq *str, loc_t loc);
    AST_Primary* mkPrimConst(AST_Literal val, loc_t loc);
    AST_Primary* mkPrimCompound(AST_TypeName *compType, AST_InitializerList *init_lst, loc_t loc);

    AST_StringsSeq* mkStringsSeq(loc_t loc);

    AST_Postfix* mkPostfArr(AST_Expr *base, AST_Expr *size, loc_t loc);
    AST_Postfix* mkPostfCall(AST_Expr *base, AST_ArgumentsList *args, loc_t loc);
    AST_Postfix* mkPostfAccesor(AST_Expr *base, string_id_t member, bool is_ptr, loc_t loc);
    AST_Postfix* mkPostfIncdec(AST_Expr *base, bool is_dec, loc_t loc);

    AST_ArgumentsList* mkArgsLst(loc_t loc);

    AST_Unop* mkUnop(AST_Unop::OpType op, AST_Expr *child, loc_t loc);
    AST_Unop* mkUnop(AST_Unop::OpType op, AST_TypeName *child, loc_t loc);

    AST_Binop* mkBinop(AST_Binop::OpType op, AST_Expr *lhs, AST_Expr *rhs, loc_t loc);

    AST_Cast* mkCastop(AST_TypeName *type, AST_Expr *child, loc_t loc);

    AST_Ternary* mkTernary(AST_Expr *cond, AST_Expr *vt, AST_Expr *vf, loc_t loc);

    AST_Assignment* mkAssign(AST_Assignment::OpType op, AST_Expr *lhs, AST_Expr *rhs, loc_t loc);

    AST_CommaExpression* mkCommaExpr(AST_Expr *expr1, AST_Expr *expr2, loc_t loc);

    // Specifiers

    AST_TypeQuals* mkTypeQuals(loc_t loc);
    AST_TypeQuals* mkTypeQuals(AST_TypeQuals::QualType init_qual, loc_t loc);

    AST_TypeSpecifier* mkTypeSpec(AST_TypeSpecifier::TypeSpec type, loc_t loc);
    AST_TypeSpecifier* mkTypeSpec(AST_UStructSpec *spec, loc_t loc);
    AST_TypeSpecifier* mkTypeSpec(AST_TypeName *spec, loc_t loc);
    AST_TypeSpecifier* mkTypeSpec(AST_EnumSpecifier *spec, loc_t loc);

    AST_DeclSpecifiers* mkDeclSpecs(loc_t loc);

    AST_SpecsQualsList* mkSpecQualLst(std::vector<AST_TypeSpecifier*> specs, AST_TypeQuals *quals, loc_t loc);
    AST_SpecsQualsList* mkSpecQualLst(AST_TypeQuals::QualType qual, loc_t loc);
    AST_SpecsQualsList* mkSpecQualLst(AST_TypeSpecifier* type, loc_t loc);

    AST_StructDeclarator* mkStructDeclarator(AST_Declarator *decl, AST_Expr *width, loc_t loc);

    AST_StructDeclaratorList* mkStructDeclaratorLst(AST_StructDeclarator *init, loc_t loc);

    AST_StructDeclaration* mkStructDeclaration(AST_SpecsQualsList *type, AST_StructDeclaratorList *child, loc_t loc);

    AST_StructDeclarationList* mkStructDeclarationLst(AST_StructDeclaration *init, loc_t loc);

    AST_UStructSpec* mkUstructSpec(bool is_union, string_id_t name, AST_StructDeclarationList *body, loc_t loc);

    AST_Enumerator* mkEnumer(string_id_t name, AST_Expr *val, loc_t loc);

    AST_EnumeratorList* mkEnumLst(AST_Enumerator *init, loc_t loc);

    AST_EnumSpecifier* mkEnumSpec(string_id_t name, AST_EnumeratorList *body, loc_t loc);

    // Declarations

    AST_InitDeclarator* mkInitDeclarator(AST_Declarator *decl, AST_Initializer *init, loc_t loc);

    AST_InitDeclaratorList* mkInitDeclaratorLst(AST_InitDeclarator *init, loc_t loc);

    AST_Declaration* mkDeclaration(AST_DeclSpecifiers *spec, AST_InitDeclaratorList *child, loc_t loc);

    AST_DirDeclarator* mkDirDeclIdent(string_id_t ident, loc_t loc);
    AST_DirDeclarator* mkDirDeclNested(AST_Declarator *decl, loc_t loc);
    AST_DirDeclarator* mkDirDeclArr(AST_DirDeclarator *base, AST_TypeQuals *qual, AST_Expr *sz, bool isStatic, loc_t loc);
    AST_DirDeclarator* mkDirDeclFunc(AST_DirDeclarator *base, AST_ParameterTypeList *args, loc_t loc);

    AST_Pointer* mkPointer(AST_TypeQuals *qual, AST_Pointer *child, loc_t loc);

    AST_Declarator* mkDeclarator(AST_DirDeclarator *decl, AST_Pointer *ptr, loc_t loc);

    AST_ParameterDeclaration* mkParamDecl(AST_DeclSpecifiers *spec, AST_Declarator *child, loc_t loc);

    AST_ParameterList* mkParamLst(AST_ParameterDeclaration *init, loc_t loc);

    AST_ParameterTypeList* mkParamTypeLst(AST_ParameterList *child, bool ellipsis, loc_t loc);

    AST_TypeName* mkTypeName(AST_SpecsQualsList *qual, std::nullptr_t decl, loc_t loc);
    AST_TypeName* mkTypeName(AST_SpecsQualsList *qual, AST_AbstrDeclarator *decl, loc_t loc);
    AST_TypeName* mkTypeName(AST_SpecsQualsList *qual, AST_Declarator *decl, loc_t loc);

    AST_DirAbstrDeclarator* mkDirAbstrDeclNested(AST_Node *decl, loc_t loc);
    AST_DirAbstrDeclarator* mkDirAbstrDeclArr(AST_Node *base, AST_Expr *sz, loc_t loc);
    AST_DirAbstrDeclarator* mkDirAbstrDeclFunc(AST_Node *base, AST_ParameterTypeList *args, loc_t loc);

    AST_AbstrDeclarator* mkAbstrDeclarator(AST_DirAbstrDeclarator *decl, AST_Pointer *pointer, loc_t loc);

    // Initializers

    AST_Designator* mkDesignator(AST_Expr *val, loc_t loc);
    AST_Designator* mkDesignator(string_id_t field, loc_t loc);

    AST_InitializerList* mkInitializerLst(AST_Initializer *init_v, AST_Designator *init_desig, loc_t loc);

    AST_Initializer* mkInitializer(AST_InitializerList *nest, loc_t loc);
    AST_Initializer* mkInitializer(AST_Expr *val, loc_t loc);

    // Statements

    AST_LabeledStmt* mkLabelStmt(AST_Expr *label, AST_Stmt *stmt, AST_LabeledStmt::LabelType type, loc_t loc);
    AST_LabeledStmt* mkLabelStmt(string_id_t label, AST_Stmt *stmt, AST_LabeledStmt::LabelType type, loc_t loc);

    AST_BlockItemList* mkBlockItemLst(loc_t loc);

    AST_CompoundStmt* mkCompoundStmt(AST_BlockItemList *body, loc_t loc);

    AST_ExprStmt* mkExprStmt(AST_Expr *child, loc_t loc);

    AST_SelectionStmt* mkIfStmt(AST_Expr *cond, AST_Stmt *body, AST_Stmt *else_body, loc_t loc);
    AST_SelectionStmt* mkSwitchStmt(AST_Expr *cond, AST_Stmt *body, loc_t loc);

    AST_IterStmt* makeWhileStmt(AST_Stmt *body, AST_Expr *ctl, bool is_do, loc_t loc);
    AST_IterStmt* makeForStmt(AST_Stmt *body, AST_Node *decl, AST_ExprStmt *cond, AST_Expr *act, loc_t loc);

    AST_JumpStmt* mkJumpStmt(AST_JumpStmt::JumpType jtype, loc_t loc);
    AST_JumpStmt* mkJumpStmt(AST_JumpStmt::JumpType jtype, AST_Expr *arg, loc_t loc);
    AST_JumpStmt* mkJumpStmt(AST_JumpStmt::JumpType jtype, string_id_t arg, loc_t loc);

    // Top-level elements

    AST_FunctionDef* mkFunDef(AST_DeclSpecifiers *spec, AST_Declarator *decl, AST_CompoundStmt *body, loc_t loc);

    AST_TranslationUnit* mkTransUnit(loc_t loc);

private:
    std::vector<std::unique_ptr<AST_Node>> nodes;

    template <typename T, typename ...Us>
    T* mkNode(loc_t loc, Us&&... args) {
        T *node = new T(std::forward<Us>(args)...);
        node->loc = std::move(loc);
        nodes.emplace_back(std::unique_ptr<AST_Node>(node));
        return node;
    }
};

#endif /* PARSER_AST_H_INCLUDED__ */
