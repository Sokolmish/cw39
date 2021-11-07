#include "ast.hpp"


// =================================================
//                    Expressions
// =================================================

// AST_Primary

AST_Primary::AST_Primary(PrimType type)
    : AST_Expr(AST_PRIMARY), type(type) {}

AST_Primary* AST_Primary::get_ident(string_id_t id) {
    auto res = new AST_Primary(AST_Primary::IDENT);
    res->v = id;
    return res;
}

AST_Primary* AST_Primary::get_expr(AST_Expr *expr) {
    auto res = new AST_Primary(AST_Primary::EXPR);
    res->v = expr;
    return res;
}

AST_Primary* AST_Primary::get_str(string_id_t str) {
    auto res = new AST_Primary(AST_Primary::STR);
    res->v = str;
    return res;
}

AST_Primary* AST_Primary::get_const(uint64_t val) {
    auto res = new AST_Primary(AST_Primary::CONST);
    res->v = val;
    return res;
}

AST_Primary::~AST_Primary() {
    if (this->type == AST_Primary::EXPR)
        delete std::get<AST_Expr*>(this->v);
}


// AST_Postfix

AST_Postfix::AST_Postfix(OpType type)
    : AST_Expr(AST_POSTFIX), op(type) {}

AST_Postfix* AST_Postfix::get_arr(AST_Expr *base, AST_Expr *size) {
    auto res = new AST_Postfix(AST_Postfix::INDEXATION);
    res->base = base;
    res->arg = size;
    return res;
}

AST_Postfix* AST_Postfix::get_call(AST_Expr *base, AST_ArgumentsList *args) {
    auto res = new AST_Postfix(AST_Postfix::CALL);
    res->base = base;
    if (args != nullptr)
        res->arg = args;
    else
        res->arg = new AST_ArgumentsList();
    return res;
}

AST_Postfix* AST_Postfix::get_accesor(AST_Expr *base, string_id_t member, bool is_ptr) {
    auto res = new AST_Postfix(is_ptr ? AST_Postfix::PTR_ACCESS : AST_Postfix::DIR_ACCESS);
    res->base = base;
    res->arg = member;
    return res;
}

AST_Postfix* AST_Postfix::get_incdec(AST_Expr *base, bool is_dec) {
    auto res = new AST_Postfix(is_dec ? AST_Postfix::POST_DEC : AST_Postfix::POST_INC);
    res->base = base;
    return res;
}

AST_Postfix::~AST_Postfix() {
    delete base;
    if (op == AST_Postfix::INDEXATION)
        delete std::get<ARR_SIZE>(this->arg);
    else if (op == AST_Postfix::CALL)
        delete std::get<CALL_ARGS>(this->arg);
}


// AST_ArgumentsList

AST_ArgumentsList::AST_ArgumentsList() : AST_Node(AST_ARGUMENTS_LIST) {}

AST_ArgumentsList* AST_ArgumentsList::append(AST_Expr *arg) {
    this->children.push_back(arg);
    return this;
}

AST_ArgumentsList::~AST_ArgumentsList() {
    for (auto &e : this->children)
        delete e;
}


// AST_Unop

AST_Unop::AST_Unop(OpType op, AST_Expr *child)
    : AST_Expr(AST_UNARY_OP), op(op), child(child) {}

AST_Unop::AST_Unop(OpType op, AST_TypeName *child)
    : AST_Expr(AST_UNARY_OP), op(op), child(child) {}

AST_Unop::~AST_Unop() {
    if (this->child.index() == CHILD_EXPR)
        delete std::get<AST_Unop::CHILD_EXPR>(this->child);
    else if (this->child.index() == TYPE_NAME)
        delete std::get<AST_Unop::TYPE_NAME>(this->child);
}


// AST_Cast

AST_Cast::AST_Cast(AST_TypeName *type, AST_Expr *child)
    : AST_Expr(AST_CAST), type_name(type), child(child) {}

AST_Cast::~AST_Cast() {
    delete this->type_name;
    delete this->child;
}


// AST_Binop

AST_Binop::AST_Binop(OpType op, AST_Expr *lhs, AST_Expr *rhs)
    : AST_Expr(AST_BINOP), op(op), lhs(lhs), rhs(rhs) {}

AST_Binop::~AST_Binop() {
    delete this->lhs;
    delete this->rhs;
}


// AST_Ternary

AST_Ternary::AST_Ternary(AST_Expr *cond, AST_Expr *vt, AST_Expr *vf)
    : AST_Expr(AST_TERNARY), cond(cond), v_true(vt), v_false(vf) {}

AST_Ternary::~AST_Ternary() {
    delete this->cond;
    delete this->v_true;
    delete this->v_false;
}


// AST_Assignment

AST_Assignment::AST_Assignment(OpType op, AST_Expr *lhs, AST_Expr *rhs)
    : AST_Expr(AST_ASSIGNMENT), op(op), lhs(lhs), rhs(rhs) {}

AST_Assignment::~AST_Assignment() {
    delete this->lhs;
    delete this->rhs;
}


// AST_CommaExpression

AST_CommaExpression::AST_CommaExpression(AST_Expr *expr)
    : AST_Expr(AST_COMMA_EXPR), children{ expr } {}

AST_CommaExpression* AST_CommaExpression::append(AST_Expr *expr) {
    this->children.push_back(expr);
    return this;
}

AST_CommaExpression::~AST_CommaExpression() {
    for (auto &e : this->children)
        delete e;
}


// =================================================
//                   Specifiers
// =================================================

// AST_TypeQualifiers

static void update_type_qualifiers(AST_TypeQualifiers &list, AST_TypeQualifiers::QualType qual) {
    switch (qual) {
        case AST_TypeQualifiers::Q_CONST:    list.is_const = true;      return;
        case AST_TypeQualifiers::Q_RESTRICT: list.is_restrict = true;   return;
        case AST_TypeQualifiers::Q_VOLATILE: list.is_volatile = true;   return;
        default: throw; // TODO: verbosity
    };
}

AST_TypeQualifiers::AST_TypeQualifiers()  : AST_Node(AST_TYPE_QUALIFIERS) {}

AST_TypeQualifiers::AST_TypeQualifiers(QualType init_qual) : AST_Node(AST_TYPE_QUALIFIERS) {
    update_type_qualifiers(*this, init_qual);
}

AST_TypeQualifiers* AST_TypeQualifiers::update(QualType new_qual) {
    update_type_qualifiers(*this, new_qual);
    return this;
}


// AST_TypeSpecifier

AST_TypeSpecifier::AST_TypeSpecifier(TypeSpec type)
    : AST_Node(AST_TYPE_SPECIFIER), spec_type(type), v(nullptr) {}

AST_TypeSpecifier::AST_TypeSpecifier(AST_StructOrUsionSpec *spec)
    : AST_Node(AST_TYPE_SPECIFIER), spec_type(AST_TypeSpecifier::T_UNISTRUCT), v(spec) {}

AST_TypeSpecifier::AST_TypeSpecifier(AST_EnumSpecifier *spec)
    : AST_Node(AST_TYPE_SPECIFIER), spec_type(AST_TypeSpecifier::T_ENUM), v(spec) {}

AST_TypeSpecifier::AST_TypeSpecifier(AST_TypeName *spec)
    : AST_Node(AST_TYPE_SPECIFIER), spec_type(AST_TypeSpecifier::T_NAMED), v(spec) {}


AST_TypeSpecifier::~AST_TypeSpecifier() {
    if (this->spec_type == T_UNISTRUCT || this->spec_type == T_ENUM)
        delete this->v;
}


//AST_DeclSpecifiers

AST_DeclSpecifiers::AST_DeclSpecifiers()
    : AST_Node(AST_DECL_SPECIFIERS), type_qualifiers(new AST_TypeQualifiers()) {}

AST_DeclSpecifiers* AST_DeclSpecifiers::update_storage(ast_enum_t val) {
    if (this->storage_specifier != ST_NONE || val == ST_NONE)
        throw; // TODO: verbosity
    this->storage_specifier = StorageSpec(val);
    return this;
}

AST_DeclSpecifiers* AST_DeclSpecifiers::update_type_spec(AST_TypeSpecifier *val) {
    this->type_specifiers.push_back(val);
    return this;
}

AST_DeclSpecifiers* AST_DeclSpecifiers::update_type_qual(ast_enum_t val) {
    this->type_qualifiers->update(AST_TypeQualifiers::QualType(val));
    return this;
}

AST_DeclSpecifiers* AST_DeclSpecifiers::update_func_qual(ast_enum_t val) {
    if (val == AST_DeclSpecifiers::Q_INLINE)
        this->is_inline = true;
    else
        throw; // TODO: verbosity
    return this;
}

AST_DeclSpecifiers::~AST_DeclSpecifiers() {
    delete this->type_qualifiers;
    for (auto &e : this->type_specifiers)
        delete e;
}


// AST_StructDeclaration

AST_StructDeclaration::AST_StructDeclaration(AST_SpecifierQualifierList *type, AST_StructDeclaratorList *child)
    : AST_Node(AST_STRUCT_DECL), type(type), child(child) {}

AST_StructDeclaration::~AST_StructDeclaration() {
    delete this->type;
    delete this->child;
}


// AST_StructDeclarator

AST_StructDeclarator::AST_StructDeclarator(AST_Declarator *decl, AST_Expr *width)
    : AST_Node(AST_STRUCT_DECLARATOR), declarator(decl), bitwidth(width) {}

AST_StructDeclarator::~AST_StructDeclarator() {
    delete this->declarator;
    delete this->bitwidth;
}


// AST_StructDeclaratorList

AST_StructDeclaratorList::AST_StructDeclaratorList(AST_StructDeclarator *init) 
    : AST_Node(AST_STRUCT_DECLARATOR_LST), children{ init } {}

AST_StructDeclaratorList* AST_StructDeclaratorList::append(AST_StructDeclarator *decl) {
    this->children.push_back(decl);
    return this;
}

AST_StructDeclaratorList::~AST_StructDeclaratorList() {
    for (auto &e : this->children)
        delete e;
}


// AST_StructDeclarationList

AST_StructDeclarationList::AST_StructDeclarationList(AST_StructDeclaration *init)
    : AST_Node(AST_STRUCT_DECL_LIST), children{ init } {}

AST_StructDeclarationList* AST_StructDeclarationList::append(AST_StructDeclaration *decl) {
    this->children.push_back(decl);
    return this;
}

AST_StructDeclarationList::~AST_StructDeclarationList() {
    for (auto &e : this->children)
        delete e;
}


// AST_SpecifierQualifierList

AST_SpecifierQualifierList::AST_SpecifierQualifierList(AST_TypeQualifiers::QualType qual) 
            : AST_Node(AST_SPEC_QUAL_LST), type_qualifiers(new AST_TypeQualifiers()) {
    this->type_qualifiers->update(qual);
}

AST_SpecifierQualifierList::AST_SpecifierQualifierList(AST_TypeSpecifier* type)
            : AST_Node(AST_SPEC_QUAL_LST), type_qualifiers(new AST_TypeQualifiers()) {
    this->type_specifiers.push_back(type);
}

AST_SpecifierQualifierList* AST_SpecifierQualifierList::append_qual(AST_TypeQualifiers::QualType qual) {
    this->type_qualifiers->update(qual);
    return this;
}

AST_SpecifierQualifierList* AST_SpecifierQualifierList::append_spec(AST_TypeSpecifier* type) {
    this->type_specifiers.push_back(type);
    return this;
}

AST_SpecifierQualifierList::~AST_SpecifierQualifierList() {
    delete this->type_qualifiers;
    for (auto &e : this->type_specifiers)
        delete e;
}


// AST_StructOrUsionSpec

AST_StructOrUsionSpec::AST_StructOrUsionSpec(bool is_uni, string_id_t name, AST_StructDeclarationList *body)
    : AST_Node(AST_STRUCT_OR_UNION_SPEC), is_union(is_uni), name(name), body(body) {}

AST_StructOrUsionSpec::~AST_StructOrUsionSpec() {
    delete this->body;
}


// AST_Enumerator

AST_Enumerator::AST_Enumerator(string_id_t name, AST_Expr *val)
    : AST_Node(AST_ENUMER), name(name), val(val) {}

AST_Enumerator::~AST_Enumerator() {
    delete this->val;
}


// AST_EnumeratorList

AST_EnumeratorList::AST_EnumeratorList(AST_Enumerator *init)
    : AST_Node(AST_ENUMER_LST), v{ init } {}

AST_EnumeratorList* AST_EnumeratorList::append(AST_Enumerator *enumer) {
    this->v.push_back(enumer);
    return this;
}

AST_EnumeratorList::~AST_EnumeratorList() {
    for (auto &e : this->v)
        delete e;
}


// AST_EnumSpecifier

AST_EnumSpecifier::AST_EnumSpecifier(string_id_t name, AST_EnumeratorList *body)
    : AST_Node(AST_ENUM_SPEC), name(name), body(body) {}

AST_EnumSpecifier::~AST_EnumSpecifier() {
    delete this->body;
}


// =================================================
//                 Declarations
// =================================================

// AST_InitDeclarator

AST_InitDeclarator::AST_InitDeclarator(AST_Declarator *decl, AST_Initializer *init)
    : AST_Node(AST_INIT_DECL), declarator(decl), initializer(init) {}

AST_InitDeclarator::~AST_InitDeclarator() {
    delete this->declarator;
    delete this->initializer;
}


// AST_InitDeclaratorList

AST_InitDeclaratorList::AST_InitDeclaratorList(AST_InitDeclarator *init)
    : AST_Node(AST_INIT_DECL_LST), v{ init } {}

AST_InitDeclaratorList* AST_InitDeclaratorList::append(AST_InitDeclarator *decl) {
    v.push_back(decl);
    return this;
}

AST_InitDeclaratorList::~AST_InitDeclaratorList() {
    for (auto &e : this->v)
        delete e;
}


// AST_Declaration

AST_Declaration::AST_Declaration(AST_DeclSpecifiers *spec, AST_InitDeclaratorList *child)
    : AST_Node(AST_DECLARATION), specifiers(spec), child(child) {}

AST_Declaration::~AST_Declaration() {
    delete this->specifiers;
    delete this->child;
}


// AST_DirectDeclarator

AST_DirectDeclarator::AST_DirectDeclarator(DeclType dtype)
    : AST_Node(AST_DIR_DECLARATOR), type(dtype) {}

AST_DirectDeclarator* AST_DirectDeclarator::AST_DirectDeclarator::get_ident(string_id_t ident) {
    auto res = new AST_DirectDeclarator(AST_DirectDeclarator::NAME);
    res->base = ident;
    return res;
}

AST_DirectDeclarator* AST_DirectDeclarator::AST_DirectDeclarator::get_nested(AST_Declarator *decl) {
    auto res = new AST_DirectDeclarator(AST_DirectDeclarator::NESTED);
    res->base = decl;
    return res;
}

AST_DirectDeclarator* AST_DirectDeclarator::get_arr(AST_DirectDeclarator *base, AST_TypeQualifiers *qual, AST_Expr *sz) {
    auto res = new AST_DirectDeclarator(AST_DirectDeclarator::ARRAY);
    res->base = base;
    res->arr_type_qual = qual;
    res->arr_size = sz;
    return res;
}

AST_DirectDeclarator* AST_DirectDeclarator::get_func(AST_DirectDeclarator *base, AST_ParameterTypeList *args) {
    auto res = new AST_DirectDeclarator(AST_DirectDeclarator::FUNC);
    res->base = base;
    res->func_args = args;
    return res;
}

AST_DirectDeclarator::~AST_DirectDeclarator() {
    if (this->type == AST_DirectDeclarator::NESTED)
        delete std::get<AST_DirectDeclarator::NEST_BASE>(this->base);
    else if (this->type == AST_DirectDeclarator::ARRAY || this->type == AST_DirectDeclarator::FUNC)
        delete std::get<AST_DirectDeclarator::COMM_BASE>(this->base);
    delete this->arr_type_qual;
    delete this->arr_size;
    delete this->func_args;
}


// AST_Pointer

AST_Pointer::AST_Pointer(AST_TypeQualifiers *qual, AST_Pointer *child)
    : AST_Node(AST_POINTER), qualifiers(qual), child(child) {}

AST_Pointer::~AST_Pointer() {
    delete this->qualifiers;
    delete this->child;
}

// AST_Declarator

AST_Declarator::AST_Declarator(AST_DirectDeclarator *decl, AST_Pointer *ptr)
    : AST_Node(AST_DECLARATOR), direct(decl), ptr(ptr) {}

AST_Declarator::~AST_Declarator() {
    delete this->direct;
    delete this->ptr;
}


// AST_ParameterDeclaration

AST_ParameterDeclaration::AST_ParameterDeclaration(AST_DeclSpecifiers *spec, AST_Node *child, bool is_abstr)
    : AST_Node(AST_PARAM_DECL), specifiers(spec), child(child), is_abstract(is_abstr) {}

AST_ParameterDeclaration::~AST_ParameterDeclaration() {
    delete this->specifiers;
    delete this->child;
}


// AST_ParameterList

AST_ParameterList::AST_ParameterList(AST_ParameterDeclaration *init)
    : AST_Node(AST_PARAM_LST), v{ init } {}

AST_ParameterList* AST_ParameterList::append(AST_ParameterDeclaration *decl) {
    this->v.push_back(decl);
    return this;
}

AST_ParameterList::~AST_ParameterList() {
    for (auto &e : this->v)
        delete e;
}


// AST_ParameterTypeList

AST_ParameterTypeList::AST_ParameterTypeList(AST_ParameterList *child, bool ellipsis)
    : AST_Node(AST_PARAM_TYPE_LST), v(child), has_ellipsis(ellipsis) {}

AST_ParameterTypeList::~AST_ParameterTypeList() {
    delete this->v;
}


// AST_TypeName

AST_TypeName::AST_TypeName(AST_SpecifierQualifierList *qual, AST_AbstractDeclarator *decl)
    : AST_Node(AST_TYPE_NAME), qual(qual), declarator(decl) {}

AST_TypeName::~AST_TypeName() {
    delete this->qual;
    delete this->declarator;
}


// AST_DirectAbstractDeclarator

AST_DirectAbstractDeclarator::AST_DirectAbstractDeclarator(DeclType dtype)
    : AST_Node(AST_DIR_ABSTRACT_DECL), type(dtype) {}

AST_DirectAbstractDeclarator* AST_DirectAbstractDeclarator::get_nested(AST_Node *decl) {
    auto res = new AST_DirectAbstractDeclarator(AST_DirectAbstractDeclarator::NESTED);
    res->base = decl;
    return res;
}
AST_DirectAbstractDeclarator* AST_DirectAbstractDeclarator::get_arr(AST_Node *base, AST_Expr *sz) {
    auto res = new AST_DirectAbstractDeclarator(AST_DirectAbstractDeclarator::ARRAY);
    res->base = base;
    res->arr_size = sz;
    return res;
}
AST_DirectAbstractDeclarator* AST_DirectAbstractDeclarator::get_func(AST_Node *base, AST_ParameterTypeList *args) {
    auto res = new AST_DirectAbstractDeclarator(AST_DirectAbstractDeclarator::FUNC);
    res->base = base;
    res->func_args = args;
    return res;
}

AST_DirectAbstractDeclarator::~AST_DirectAbstractDeclarator() {
    delete this->base;
    delete this->arr_size;
    delete this->func_args;
}


// AST_AbstractDeclarator

AST_AbstractDeclarator::AST_AbstractDeclarator(AST_DirectAbstractDeclarator *decl, AST_Pointer *pointer)
    : AST_Node(AST_ABSTRACT_DECL), direct(decl), ptr(pointer) {}

AST_AbstractDeclarator::~AST_AbstractDeclarator() {
    delete this->direct;
    delete this->ptr;
}


// =================================================
//                 Initializers
// =================================================

// AST_Designator

AST_Designator::AST_Designator(AST_Expr *val)
    : AST_Node(AST_DESIGNATOR), val(val), is_index(true) {}

AST_Designator::AST_Designator(string_id_t field)
    : AST_Node(AST_DESIGNATOR), val(field), is_index(false) {}

AST_Designator::~AST_Designator() {
    if (this->is_index)
        delete std::get<AST_Designator::INDEX>(this->val);
}


// AST_InitializerList::AST_InitializerListElem

AST_InitializerList::AST_InitializerListElem::AST_InitializerListElem(AST_Initializer *v, AST_Designator *desig)
    : val(v), designator(desig) {}

AST_InitializerList::AST_InitializerListElem::~AST_InitializerListElem() {
    delete this->val;
    delete this->designator;
}


// AST_InitializerList

AST_InitializerList::AST_InitializerList(AST_Initializer *init_v, AST_Designator *init_desig)
    : AST_Node(AST_INITIALIZER_LST), v{ { init_v, init_desig } } {}

AST_InitializerList* AST_InitializerList::append(AST_Initializer *v, AST_Designator *desig) {
    this->v.push_back({ v, desig });
    return this;
}


// AST_Initializer

AST_Initializer::AST_Initializer(AST_InitializerList *nest)
    : AST_Node(AST_INITIALIZER), is_compound(true), val(nest) {}

AST_Initializer::AST_Initializer(AST_Expr *val)
    : AST_Node(AST_INITIALIZER), is_compound(false), val(val) {}

AST_Initializer::~AST_Initializer() {
    if (this->is_compound)
        delete std::get<AST_Initializer::COMPOUND>(this->val);
    else
        delete std::get<AST_Initializer::DIRECT>(this->val);
}


// =================================================
//                   Statements
// =================================================

// AST_Statement

AST_Statement::AST_Statement(StmtType stype) : AST_Node(AST_STATEMENT), type(stype) {}


// AST_LabeledStmt

AST_LabeledStmt::AST_LabeledStmt(AST_Node *label, AST_Statement *stmt, LabelType type)
    : AST_Statement(AST_Statement::LABEL), label(label), child(stmt), type(type)
{
    if (type == AST_LabeledStmt::SIMPL)
        throw; // TODO: verbosity
}

AST_LabeledStmt::AST_LabeledStmt(string_id_t label, AST_Statement *stmt, LabelType type)
    : AST_Statement(AST_Statement::LABEL), label(label), child(stmt), type(type) 
{
    if (type != AST_LabeledStmt::SIMPL)
        throw; // TODO: verbosity
}

AST_LabeledStmt::~AST_LabeledStmt() {
    if (this->label.index() == AST_LabeledStmt::CASE_EXPR)
        delete std::get<AST_LabeledStmt::CASE_EXPR>(this->label);
    delete this->child;
}


// AST_BlockItemList

AST_BlockItemList::AST_BlockItemList()
    : AST_Node(AST_BLOCK_ITEM_LST), v() {}

AST_BlockItemList* AST_BlockItemList::append(AST_Node *child) {
    this->v.push_back(child);
    return this;
}

AST_BlockItemList::~AST_BlockItemList() {
    for (auto &e : this->v)
        delete e;
}


// AST_CompoundStmt

AST_CompoundStmt::AST_CompoundStmt(AST_BlockItemList *body)
    : AST_Statement(AST_Statement::COMPOUND), body(body) {}

AST_CompoundStmt::~AST_CompoundStmt() {
    delete this->body;
}


// AST_ExprStmt

AST_ExprStmt::AST_ExprStmt(AST_Expr *child)
    : AST_Statement(AST_Statement::EXPR), child(child) {}

AST_ExprStmt::~AST_ExprStmt() {
    delete this->child;
}


// AST_SelectionStmt

AST_SelectionStmt::AST_SelectionStmt(bool sw)
    : AST_Statement(AST_Statement::SELECT), is_switch(sw) {}

AST_SelectionStmt* AST_SelectionStmt::get_if(AST_Expr *cond, AST_Statement *body, AST_Statement *else_body) {
    auto res = new AST_SelectionStmt(false);
    res->condition = cond;
    res->body = body;
    res->else_body = else_body;
    return res;    
}

AST_SelectionStmt* AST_SelectionStmt::get_switch(AST_Expr *cond, AST_Statement *body) {
    auto res = new AST_SelectionStmt(true);
    res->condition = cond;
    res->body = body;
    res->else_body = nullptr;
    return res;
}

AST_SelectionStmt::~AST_SelectionStmt() {
    delete this->condition;
    delete this->body;
    delete this->else_body;
}


// AST_IterationStmt

AST_IterationStmt::AST_IterationStmt(LoopType ltype, AST_Statement *body)
    : AST_Statement(AST_Statement::ITER), type(ltype), body(body) {}

AST_IterationStmt* AST_IterationStmt::get_while(AST_Statement *body, AST_Expr *ctl, bool is_do) {
    AST_IterationStmt::LoopType ltype = is_do ? AST_IterationStmt::DO_LOOP : AST_IterationStmt::WHILE_LOOP;
    auto res = new AST_IterationStmt(ltype, body);
    res->control = ctl;
    return res;
}

AST_IterationStmt* AST_IterationStmt::get_for(AST_Statement *body, AST_Node *dcl, AST_ExprStmt *cond, AST_Expr *act) {
    auto res = new AST_IterationStmt(AST_IterationStmt::FOR_LOOP, body);
    res->control = new AST_ForLoopControls{ dcl, cond, act };
    return res;
}

AST_IterationStmt::~AST_IterationStmt() {
    delete this->body;
    if (this->type == AST_IterationStmt::FOR_LOOP) {
        auto ctl = std::get<AST_IterationStmt::FOR_CTL>(this->control);
        delete ctl->decl;
        delete ctl->cond;
        delete ctl->action;
        delete ctl;
    }
    else {
        delete std::get<AST_IterationStmt::WHILE_CTL>(this->control);
    }
}


// AST_JumpStmt

AST_JumpStmt::AST_JumpStmt(JumpType jtype)
    : AST_Statement(AST_Statement::JUMP), type(jtype), arg(nullptr) {}

AST_JumpStmt::AST_JumpStmt(JumpType jtype, AST_Expr *arg)
    : AST_Statement(AST_Statement::JUMP), type(jtype), arg(arg) {}

AST_JumpStmt::AST_JumpStmt(JumpType jtype, string_id_t arg)
    : AST_Statement(AST_Statement::JUMP), type(jtype), arg(arg) {}

AST_JumpStmt::~AST_JumpStmt() {
    if (this->arg.index() == AST_JumpStmt::RET_EXPR)
        delete std::get<AST_JumpStmt::RET_EXPR>(this->arg);
}


// =================================================
//                Top-level elements
// =================================================

// AST_FunctionDef

AST_FunctionDef::AST_FunctionDef(AST_DeclSpecifiers *spec, AST_Declarator *decl, AST_CompoundStmt *body)
    : AST_Node(AST_FUNC_DEF), specifiers(spec), decl(decl), body(body) {}

AST_FunctionDef::~AST_FunctionDef() {
    delete this->specifiers;
    delete this->decl;
    delete this->body;
}


// AST_TranslationUnit

AST_TranslationUnit::AST_TranslationUnit() : AST_Node(AST_TRANS_UNIT), children() {}

AST_TranslationUnit* AST_TranslationUnit::append(AST_Node *node) {
    this->children.push_back(node);
    return this;
}

AST_TranslationUnit::~AST_TranslationUnit() {
    for (auto &e : this->children)
        delete e;
}
