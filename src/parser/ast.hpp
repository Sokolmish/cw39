#ifndef __PARSER_AST_H__
#define __PARSER_AST_H__

#include <cstdint>
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include "common.h"
#include "print_tree.hpp"

typedef int ast_enum_t;

enum : ast_enum_t {
    AST_PRIMARY, AST_POSTFIX, AST_ARGUMENTS_LIST, AST_UNARY_OP, AST_CAST, AST_BINOP,
    AST_TERNARY, AST_ASSIGNMENT, AST_COMMA_EXPR,
    AST_TYPE_QUALIFIERS, AST_TYPE_SPECIFIER, AST_DECL_SPECIFIERS,
    AST_STRUCT_OR_UNION_SPEC, AST_STRUCT_DECL_LIST, AST_STRUCT_DECL,
    AST_SPEC_QUAL_LST, AST_STRUCT_DECLARATOR, AST_STRUCT_DECLARATOR_LST,
    AST_ENUM_SPEC, AST_ENUMER, AST_ENUMER_LST,
    AST_DECLARATION, AST_INIT_DECL_LST, AST_INIT_DECL, AST_DECLARATOR,
    AST_DIR_DECLARATOR, AST_POINTER, AST_PARAM_DECL, AST_PARAM_TYPE_LST, AST_PARAM_LST,
    AST_TYPE_NAME, AST_DESIGNATOR, AST_INITIALIZER_LST, AST_INITIALIZER,
    AST_STATEMENT, AST_BLOCK_ITEM_LST, AST_FUNC_DEF, AST_TRANS_UNIT,
    AST_DIR_ABSTRACT_DECL, AST_ABSTRACT_DECL,
};

struct AST_Node {
    AST_Node(AST_Node const &) = delete;
    AST_Node& operator=(AST_Node const &) = delete;

    int node_type;

    explicit AST_Node(int type) : node_type(type) {}
    virtual ~AST_Node() = default;
    [[nodiscard]] virtual TreeNodeRef getTreeNode() const = 0;

protected:
    template <typename T>
    using uniq = std::unique_ptr<T>;

    template <typename T>
    static std::unique_ptr<T> uniqify(T *raw) {
        return std::unique_ptr<T>(raw);
    }
};

struct AST_StructOrUsionSpec;
struct AST_EnumSpecifier;
struct AST_Declarator;
struct AST_AbstractDeclarator;
struct AST_TypeName;
struct AST_Initializer;
struct AST_ParameterTypeList;
struct AST_Statement;


// =================================================
//                    Expressions
// =================================================

struct AST_Expr : public AST_Node {
    explicit AST_Expr(int type) : AST_Node(type) {}
};

struct AST_Primary : public AST_Expr {
    enum PrimType : ast_enum_t { IDENT, EXPR, STR, CONST } type;
    std::variant<uniq<AST_Expr>, string_id_t, AST_Literal> v;

    string_id_t getIdent() const;
    string_id_t getString() const;
    AST_Literal getLiteral() const;
    AST_Expr const& getExpr() const;

    static AST_Primary* makeIdent(string_id_t id);
    static AST_Primary* makeExpr(AST_Expr *expr);
    static AST_Primary* makeStr(string_id_t str);
    static AST_Primary* makeConst(AST_Literal val);

    [[nodiscard]] TreeNodeRef getTreeNode() const override;

private:
    explicit AST_Primary(PrimType type);
};

struct AST_ArgumentsList : public AST_Node {
    std::vector<uniq<AST_Expr>> children;

    AST_ArgumentsList();
    AST_ArgumentsList* append(AST_Expr *arg);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_Postfix : public AST_Expr {
    enum OpType : ast_enum_t {
        CALL, POST_INC, POST_DEC, INDEXATION, DIR_ACCESS, PTR_ACCESS
    } op;
    uniq<AST_Expr> base = nullptr;
    std::variant<uniq<AST_Node>, string_id_t> arg; // Expr ArgumentsList

    AST_Expr const& getExpr() const; // TODO: getArgExpr
    AST_ArgumentsList const& getArgsList() const;
    string_id_t getIdent() const;

    static AST_Postfix* makeArr(AST_Expr *base, AST_Expr *size);
    static AST_Postfix* makeCall(AST_Expr *base, AST_ArgumentsList *args);
    static AST_Postfix* makeAccesor(AST_Expr *base, string_id_t member, bool is_ptr);
    static AST_Postfix* makeIncdec(AST_Expr *base, bool is_dec);

    [[nodiscard]] TreeNodeRef getTreeNode() const override;

private:
    explicit AST_Postfix(OpType type);
};

struct AST_Unop : public AST_Expr {
    enum OpType : ast_enum_t {
        PRE_INC, PRE_DEC,
        ADDR_OF, DEREF, UN_PLUS, UN_MINUS, UN_NEG, UN_NOT,
        SIZEOF_OP,
    } op;
    uniq<AST_Node> child; // Expr TypeName

    AST_Unop(OpType op, AST_Expr *child);
    AST_Unop(OpType op, AST_TypeName *child);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_Cast : public AST_Expr {
    uniq<AST_TypeName> type_name;
    uniq<AST_Expr> child;

    AST_Cast(AST_TypeName *type, AST_Expr *child);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_Binop : public AST_Expr {
    static constexpr const char * const opstr[] = {
            "*", "/", "%", "+", "-", "<<", ">>",
            "<", ">", "<=", ">=", "==", "!=",
            "&", "^", "|", "&&", "||"
    };

    enum OpType : ast_enum_t {
        MUL, DIV, REM, ADD, SUB, SHL, SHR,
        LT, GT, LE, GE, EQ, NE,
        BIT_AND, BIT_XOR, BIT_OR, LOG_AND, LOG_OR
    } op;
    uniq<AST_Expr> lhs, rhs;

    AST_Binop(OpType op, AST_Expr *lhs, AST_Expr *rhs);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_Ternary : public AST_Expr {
    uniq<AST_Expr> cond, v_true, v_false;

    AST_Ternary(AST_Expr *cond, AST_Expr *vt, AST_Expr *vf);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_Assignment : public AST_Expr {
    static constexpr const char * const opstr[] = {
            "=", "*=", "/=", "%=", "+=", "-=", "<<=", ">>=",
            "&=", "^=", "|="
    };

    enum OpType : ast_enum_t {
        DIRECT, MUL, DIV, REM, ADD, SUB, SHL, SHR, AND, XOR, OR
    } op;
    uniq<AST_Expr> lhs, rhs;

    std::optional<AST_Binop::OpType> toBinop() const;

    AST_Assignment(OpType op, AST_Expr *lhs, AST_Expr *rhs);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_CommaExpression : public AST_Expr {
    std::vector<uniq<AST_Expr>> children;

    explicit AST_CommaExpression(AST_Expr *expr);
    AST_CommaExpression* append(AST_Expr *expr);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};


// =================================================
//                   Specifiers
// =================================================

struct AST_TypeQualifiers : public AST_Node {
    enum QualType : ast_enum_t { Q_CONST, Q_RESTRICT, Q_VOLATILE };
    bool is_const, is_restrict, is_volatile;

    AST_TypeQualifiers();
    explicit AST_TypeQualifiers(QualType init_qual);
    AST_TypeQualifiers* update(QualType new_qual);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_TypeSpecifier : public AST_Node {
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
    uniq<AST_Node> v; // AST_StructOrUsionSpec AST_TypeName AST_EnumSpecifier

    explicit AST_TypeSpecifier(TypeSpec type);
    explicit AST_TypeSpecifier(AST_StructOrUsionSpec *spec);
    explicit AST_TypeSpecifier(AST_TypeName *spec);
    explicit AST_TypeSpecifier(AST_EnumSpecifier *spec);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_DeclSpecifiers : public AST_Node {
    static constexpr const char * const storstr[] = {
            "<storage>", "extern", "static", "auto",
            "register", "typedef",
    };

    enum StorageSpec : ast_enum_t {
        ST_NONE, ST_EXTERN, ST_STATIC, ST_AUTO, ST_REGISTER, ST_TYPEDEF
    };
    enum FuncQual : ast_enum_t { Q_INLINE };

    StorageSpec storage_specifier = ST_NONE;
    std::vector<uniq<AST_TypeSpecifier>> type_specifiers;
    uniq<AST_TypeQualifiers> type_qualifiers;
    bool is_inline = false;

    AST_DeclSpecifiers();
    AST_DeclSpecifiers* update_storage(ast_enum_t val);
    AST_DeclSpecifiers* update_type_spec(AST_TypeSpecifier *val);
    AST_DeclSpecifiers* update_type_qual(ast_enum_t val);
    AST_DeclSpecifiers* update_func_qual(ast_enum_t val);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_SpecifierQualifierList : public AST_Node {
    std::vector<uniq<AST_TypeSpecifier>> type_specifiers;
    uniq<AST_TypeQualifiers> type_qualifiers = nullptr;

    explicit AST_SpecifierQualifierList(AST_TypeQualifiers::QualType qual);
    explicit AST_SpecifierQualifierList(AST_TypeSpecifier* type);
    AST_SpecifierQualifierList* append_qual(AST_TypeQualifiers::QualType qual);
    AST_SpecifierQualifierList* append_spec(AST_TypeSpecifier* type);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_StructDeclarator : public AST_Node {
    uniq<AST_Declarator> declarator;
    uniq<AST_Expr> bitwidth;

    AST_StructDeclarator(AST_Declarator *decl, AST_Expr *width);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_StructDeclaratorList : public AST_Node {
    std::vector<uniq<AST_StructDeclarator>> children;

    explicit AST_StructDeclaratorList(AST_StructDeclarator *init);
    AST_StructDeclaratorList* append(AST_StructDeclarator *decl);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_StructDeclaration : public AST_Node {
    uniq<AST_SpecifierQualifierList> type;
    uniq<AST_StructDeclaratorList> child;

    AST_StructDeclaration(AST_SpecifierQualifierList *type, AST_StructDeclaratorList *child);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_StructDeclarationList : public AST_Node {
    std::vector<uniq<AST_StructDeclaration>> children;

    explicit AST_StructDeclarationList(AST_StructDeclaration *init);
    AST_StructDeclarationList* append(AST_StructDeclaration *decl);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_StructOrUsionSpec : public AST_Node {
    bool is_union;
    string_id_t name;
    uniq<AST_StructDeclarationList> body;

    AST_StructOrUsionSpec(bool is_union, string_id_t name, AST_StructDeclarationList *body);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_Enumerator : public AST_Node {
    string_id_t name;
    uniq<AST_Expr> val;

    AST_Enumerator(string_id_t name, AST_Expr *val);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_EnumeratorList : public AST_Node {
    std::vector<uniq<AST_Enumerator>> v;

    explicit AST_EnumeratorList(AST_Enumerator *init);
    AST_EnumeratorList* append(AST_Enumerator *enumer);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_EnumSpecifier : public AST_Node {
    string_id_t name;
    uniq<AST_EnumeratorList> body;

    AST_EnumSpecifier(string_id_t name, AST_EnumeratorList *body);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};


// =================================================
//                 Declarations
// =================================================

struct AST_InitDeclarator : public AST_Node {
    uniq<AST_Declarator> declarator;
    uniq<AST_Initializer> initializer;

    AST_InitDeclarator(AST_Declarator *decl, AST_Initializer *init);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_InitDeclaratorList : public AST_Node {
    std::vector<uniq<AST_InitDeclarator>> v;

    explicit AST_InitDeclaratorList(AST_InitDeclarator *init);
    AST_InitDeclaratorList* append(AST_InitDeclarator *decl);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_Declaration : public AST_Node {
    uniq<AST_DeclSpecifiers> specifiers;
    uniq<AST_InitDeclaratorList> child;

    AST_Declaration(AST_DeclSpecifiers *spec, AST_InitDeclaratorList *child);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_DirectDeclarator : public AST_Node {
    enum DeclType : ast_enum_t { NAME, NESTED, ARRAY, FUNC } type;
    std::variant<string_id_t, uniq<AST_Node>> base; // DirectDeclarator Declarator

    uniq<AST_TypeQualifiers> arr_type_qual = nullptr;
    uniq<AST_Expr> arr_size = nullptr;
    uniq<AST_ParameterTypeList> func_args = nullptr;

    string_id_t getIdent() const;
    AST_DirectDeclarator const& getBaseDirectDecl() const;
    AST_Declarator const& getBaseDecl() const;

    static AST_DirectDeclarator* makeIdent(string_id_t ident);
    static AST_DirectDeclarator* makeNested(AST_Declarator *decl);
    static AST_DirectDeclarator* makeArr(AST_DirectDeclarator *base,
                                         AST_TypeQualifiers *qual, AST_Expr *sz);
    static AST_DirectDeclarator* makeFunc(AST_DirectDeclarator *base,
                                          AST_ParameterTypeList *args);

    [[nodiscard]] TreeNodeRef getTreeNode() const override;

private:
    explicit AST_DirectDeclarator(DeclType dtype);
};

struct AST_Pointer : public AST_Node {
    uniq<AST_TypeQualifiers> qualifiers;
    uniq<AST_Pointer> child;

    AST_Pointer(AST_TypeQualifiers *qual, AST_Pointer *child);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_Declarator : public AST_Node {
    uniq<AST_DirectDeclarator> direct;
    uniq<AST_Pointer> ptr;

    AST_Declarator(AST_DirectDeclarator *decl, AST_Pointer *ptr);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_ParameterDeclaration : public AST_Node {
    uniq<AST_DeclSpecifiers> specifiers;
    uniq<AST_Declarator> child; // Declarator

    AST_ParameterDeclaration(AST_DeclSpecifiers *spec, AST_Declarator *child);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_ParameterList : public AST_Node {
    std::vector<uniq<AST_ParameterDeclaration>> v;

    explicit AST_ParameterList(AST_ParameterDeclaration *init);
    AST_ParameterList* append(AST_ParameterDeclaration *decl);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_ParameterTypeList : public AST_Node {
    uniq<AST_ParameterList> v;
    bool has_ellipsis;

    AST_ParameterTypeList(AST_ParameterList *child, bool ellipsis);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_TypeName : public AST_Node {
    uniq<AST_SpecifierQualifierList> qual;
    uniq<AST_AbstractDeclarator> declarator;

    AST_TypeName(AST_SpecifierQualifierList *qual, AST_AbstractDeclarator *decl);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_DirectAbstractDeclarator : public AST_Node {
    enum DeclType : ast_enum_t { NESTED, ARRAY, FUNC } type;
    uniq<AST_Node> base = nullptr;

    uniq<AST_Expr> arr_size = nullptr;
    uniq<AST_ParameterTypeList> func_args = nullptr;

    AST_DirectAbstractDeclarator const& getBaseDirectDecl() const;
    AST_AbstractDeclarator const& getBaseDecl() const;

    static AST_DirectAbstractDeclarator* makeNested(AST_Node *decl);
    static AST_DirectAbstractDeclarator* makeArr(AST_Node *base, AST_Expr *sz);
    static AST_DirectAbstractDeclarator* makeFunc(AST_Node *base, AST_ParameterTypeList *args);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;

private:
    explicit AST_DirectAbstractDeclarator(DeclType dtype);
};

struct AST_AbstractDeclarator : public AST_Node {
    uniq<AST_DirectAbstractDeclarator> direct;
    uniq<AST_Pointer> ptr;

    AST_AbstractDeclarator(AST_DirectAbstractDeclarator *decl, AST_Pointer *pointer);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};


// =================================================
//                 Initializers
// =================================================

struct AST_Designator : public AST_Node {
    std::variant<uniq<AST_Expr>, string_id_t> val;
    bool is_index; // indexation or field name

    explicit AST_Designator(AST_Expr *val);
    explicit AST_Designator(string_id_t field);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_InitializerList : public AST_Node {
    struct AST_InitializerListElem {
        uniq<AST_Initializer> val;
        uniq<AST_Designator> designator;

        AST_InitializerListElem(AST_Initializer *v, AST_Designator *desig);
        [[nodiscard]] TreeNodeRef getTreeNode() const;
    };

    std::vector<AST_InitializerListElem> children;

    AST_InitializerList(AST_Initializer *init_v, AST_Designator *init_desig);
    AST_InitializerList* append(AST_Initializer *val, AST_Designator *desig);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_Initializer : public AST_Node {
    bool is_compound;
    uniq<AST_Node> val; // AST_InitializerList AST_Expr

    AST_Expr const& getExpr() const;
    AST_InitializerList const& getInitList() const;

    explicit AST_Initializer(AST_InitializerList *nest);
    explicit AST_Initializer(AST_Expr *val);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};


// =================================================
//                   Statements
// =================================================

struct AST_Statement : public AST_Node {
    enum StmtType : ast_enum_t {
        LABEL, COMPOUND, EXPR, SELECT, ITER, JUMP
    } type;
    explicit AST_Statement(StmtType stype);
};

struct AST_LabeledStmt : public AST_Statement {
    std::variant<uniq<AST_Expr>, string_id_t> label;
    uniq<AST_Statement> child;
    enum LabelType : ast_enum_t { SIMPL, SW_CASE, SW_DEFAULT } type;

    AST_LabeledStmt(AST_Expr *label, AST_Statement *stmt, LabelType type);
    AST_LabeledStmt(string_id_t label, AST_Statement *stmt, LabelType type);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;

    string_id_t getIdent() const;
    AST_Expr const& getExpr() const;
};

struct AST_BlockItemList : public AST_Node {
    std::vector<uniq<AST_Node>> v; // Declaration Statement

    AST_BlockItemList();
    AST_BlockItemList* append(AST_Node *child);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_CompoundStmt : public AST_Statement {
    uniq<AST_BlockItemList> body;

    explicit AST_CompoundStmt(AST_BlockItemList *body);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_ExprStmt : public AST_Statement {
    uniq<AST_Expr> child;

    explicit AST_ExprStmt(AST_Expr *child);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_SelectionStmt : public AST_Statement {
    bool is_switch;
    uniq<AST_Expr> condition = nullptr;
    uniq<AST_Statement> body = nullptr;
    uniq<AST_Statement> else_body = nullptr;

    static AST_SelectionStmt* get_if(AST_Expr *cond, AST_Statement *body, AST_Statement *else_body);
    static AST_SelectionStmt* get_switch(AST_Expr *cond, AST_Statement *body);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;

private:
    explicit AST_SelectionStmt(bool sw);
};

struct AST_IterationStmt : public AST_Statement {
    struct ForLoopControls {
        uniq<AST_Node> decl;
        uniq<AST_ExprStmt> cond;
        uniq<AST_Expr> action;

        [[nodiscard]] TreeNodeRef getTreeNode() const;
    };

    enum LoopType : ast_enum_t { WHILE_LOOP, DO_LOOP, FOR_LOOP } type;
    uniq<AST_Statement> body;
    std::variant<uniq<AST_Expr>, ForLoopControls> control;

    std::unique_ptr<AST_Expr> const& getCond() const;
    ForLoopControls const& getForLoopControls() const;

    static AST_IterationStmt* makeWhileLoop(AST_Statement *body, AST_Expr *ctl, bool is_do);
    static AST_IterationStmt* makeForLoop(AST_Statement *body, AST_Node *decl,
                                          AST_ExprStmt *cond, AST_Expr *act);

    [[nodiscard]] TreeNodeRef getTreeNode() const override;

private:
    AST_IterationStmt(LoopType ltype, AST_Statement *body);
};

struct AST_JumpStmt : public AST_Statement {
    enum JumpType : ast_enum_t {
        J_GOTO, J_CONTINUE, J_BREAK, J_RET
    } type;
    std::variant<uniq<AST_Expr>, string_id_t> arg;

    explicit AST_JumpStmt(JumpType jtype);
    AST_JumpStmt(JumpType jtype, AST_Expr *arg);
    AST_JumpStmt(JumpType jtype, string_id_t arg);

    std::unique_ptr<AST_Expr> const& getExpr() const;
    string_id_t getIdent() const;

    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};


// =================================================
//                Top-level elements
// =================================================

struct AST_FunctionDef : public AST_Node {
    uniq<AST_DeclSpecifiers> specifiers;
    uniq<AST_Declarator> decl;
    uniq<AST_CompoundStmt> body;

    AST_FunctionDef(AST_DeclSpecifiers *spec, AST_Declarator *decl, AST_CompoundStmt *body);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

struct AST_TranslationUnit : public AST_Node {
    std::vector<uniq<AST_Node>> children;

    AST_TranslationUnit();
    AST_TranslationUnit* append(AST_Node *node);
    [[nodiscard]] TreeNodeRef getTreeNode() const override;
};

// Implemented in utils.cpp
void check_typedef(AST_Declaration *decl);
AST_TypeName* get_def_type(string_id_t id);

#endif /* __PARSER_AST_H__ */
