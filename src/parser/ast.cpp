#include "ast.hpp"
#include "parser.hpp"
#include <memory>
#include <string>
#include <fmt/core.h>

using namespace std::string_literals;


// =================================================
//                    Expressions
// =================================================

// AST_Primary

AST_Primary::AST_Primary(PrimType type)
    : AST_Expr(AST_PRIMARY), type(type) {}

AST_Primary* AST_Primary::makeIdent(string_id_t id) {
    auto res = new AST_Primary(AST_Primary::IDENT);
    res->v = id;
    return res;
}

AST_Primary* AST_Primary::makeExpr(AST_Expr *expr) {
    auto res = new AST_Primary(AST_Primary::EXPR);
    res->v = uniqify(expr);
    return res;
}

AST_Primary* AST_Primary::makeStr(string_id_t str) {
    auto res = new AST_Primary(AST_Primary::STR);
    res->v = str;
    return res;
}

AST_Primary* AST_Primary::makeConst(AST_Literal val) {
    auto res = new AST_Primary(AST_Primary::CONST);
    res->v = val;
    return res;
}

string_id_t AST_Primary::getIdent() const {
    return std::get<string_id_t>(v);
}

string_id_t AST_Primary::getString() const {
    return std::get<string_id_t>(v);
}

AST_Literal AST_Primary::getLiteral() const {
    return std::get<AST_Literal>(v);
}

AST_Expr const& AST_Primary::getExpr() const {
    return *std::get<std::unique_ptr<AST_Expr>>(v);
}

TreeNodeRef AST_Primary::getTreeNode() const {
    if (type != AST_Primary::EXPR) {
        std::string str;
        if (type == AST_Primary::IDENT)
            str = get_ident_by_id(std::get<string_id_t>(v));
        else if (type == AST_Primary::CONST)
//            str = "val="s + std::to_string(std::get<uint64_t>(v));
            str = "LITERAL"; // TODO
        else if (type == AST_Primary::STR)
            str = "str["s + std::to_string(std::get<string_id_t>(v)) + "]"s;
        return TreeNode::create(str);
    }
    else {
        return std::get<uniq<AST_Expr>>(v)->getTreeNode();
    }
}


// AST_Postfix

AST_Postfix::AST_Postfix(OpType type)
    : AST_Expr(AST_POSTFIX), op(type) {}

AST_Postfix* AST_Postfix::makeArr(AST_Expr *base, AST_Expr *size) {
    auto res = new AST_Postfix(AST_Postfix::INDEXATION);
    res->base = uniqify(base);
    res->arg = uniqify(size);
    return res;
}

AST_Postfix* AST_Postfix::makeCall(AST_Expr *base, AST_ArgumentsList *args) {
    auto res = new AST_Postfix(AST_Postfix::CALL);
    res->base = uniqify(base);
    if (args != nullptr)
        res->arg = uniqify(args);
    else
        res->arg = std::make_unique<AST_ArgumentsList>();
    return res;
}

AST_Postfix* AST_Postfix::makeAccesor(AST_Expr *base, string_id_t member, bool is_ptr) {
    auto res = new AST_Postfix(is_ptr ? AST_Postfix::PTR_ACCESS : AST_Postfix::DIR_ACCESS);
    res->base = uniqify(base);
    res->arg = member;
    return res;
}

AST_Postfix* AST_Postfix::makeIncdec(AST_Expr *base, bool is_dec) {
    auto res = new AST_Postfix(is_dec ? AST_Postfix::POST_DEC : AST_Postfix::POST_INC);
    res->base = uniqify(base);
    return res;
}

TreeNodeRef AST_Postfix::getTreeNode() const {
    if (op == AST_Postfix::INDEXATION) {
        auto node = TreeNode::create("arr_at[]"s);
        node->addChild(base->getTreeNode());
        node->addChild(std::get<uniq<AST_Node>>(arg)->getTreeNode());
        return node;
    }
    else if (op == AST_Postfix::CALL) {
        auto node = TreeNode::create("call"s);
        node->addChild(base->getTreeNode());
        node->addChild(std::get<uniq<AST_Node>>(arg)->getTreeNode());
        return node;
    }

    std::string str;
    if (op == AST_Postfix::DIR_ACCESS)
        str = "."s + get_ident_by_id(std::get<string_id_t>(arg));
    else if (op == AST_Postfix::PTR_ACCESS)
        str = "->"s + get_ident_by_id(std::get<string_id_t>(arg));
    else if (op == AST_Postfix::POST_INC)
        str = "()++"s;
    else if (op == AST_Postfix::POST_DEC)
        str = "()--"s;
    auto node = TreeNode::create(str);
    node->addChild(base->getTreeNode());
    return node;
}

AST_Expr const& AST_Postfix::getExpr() const {
    auto const &uptr = std::get<std::unique_ptr<AST_Node>>(arg);
    return dynamic_cast<AST_Expr const &>(*uptr);
}

AST_ArgumentsList const &AST_Postfix::getArgsList() const {
    auto const &uptr = std::get<std::unique_ptr<AST_Node>>(arg);
    return dynamic_cast<AST_ArgumentsList const &>(*uptr);
}

string_id_t AST_Postfix::getIdent() const {
    return std::get<string_id_t>(arg);
}


// AST_ArgumentsList

AST_ArgumentsList::AST_ArgumentsList() : AST_Node(AST_ARGUMENTS_LIST) {}

AST_ArgumentsList* AST_ArgumentsList::append(AST_Expr *arg) {
    children.emplace_back(arg);
    return this;
}

TreeNodeRef AST_ArgumentsList::getTreeNode() const {
    auto node = TreeNode::create("args"s);
    for (auto &e : children)
        node->addChild(e->getTreeNode());
    return node;
}


// AST_Unop

AST_Unop::AST_Unop(OpType op, AST_Expr *child)
    : AST_Expr(AST_UNARY_OP), op(op), child(child) {}

AST_Unop::AST_Unop(OpType op, AST_TypeName *child)
    : AST_Expr(AST_UNARY_OP), op(op), child(child) {}

TreeNodeRef AST_Unop::getTreeNode() const {
    if (op == AST_Unop::SIZEOF_OP) {
        // TODO
        return TreeNode::create("sizeof"s);
    }

    std::string str;
    if (op == AST_Unop::PRE_INC) str = "++()"s;
    else if (op == AST_Unop::PRE_DEC) str = "--()"s;
    else if (op == AST_Unop::ADDR_OF) str = "&()"s;
    else if (op == AST_Unop::DEREF) str = "*()"s;
    else if (op == AST_Unop::UN_PLUS) str = "+()"s;
    else if (op == AST_Unop::UN_MINUS) str = "-()"s;
    else if (op == AST_Unop::UN_NEG) str = "~()"s;
    else if (op == AST_Unop::UN_NOT) str = "!()"s;

    auto node = TreeNode::create(str);
    node->addChild(child->getTreeNode());
    return node;
}


// AST_Cast

AST_Cast::AST_Cast(AST_TypeName *type, AST_Expr *child)
    : AST_Expr(AST_CAST), type_name(type), child(child) {}

TreeNodeRef AST_Cast::getTreeNode() const {
    auto node = TreeNode::create("cast"s);
    node->addChild(type_name->getTreeNode());
    node->addChild(child->getTreeNode());
    return node;
}


// AST_Binop

AST_Binop::AST_Binop(OpType op, AST_Expr *lhs, AST_Expr *rhs)
    : AST_Expr(AST_BINOP), op(op), lhs(lhs), rhs(rhs) {}

TreeNodeRef AST_Binop::getTreeNode() const {
    auto node = TreeNode::create(opstr[op]);
    node->addChild(lhs->getTreeNode());
    node->addChild(rhs->getTreeNode());
    return node;
}


// AST_Ternary

AST_Ternary::AST_Ternary(AST_Expr *cond, AST_Expr *vt, AST_Expr *vf)
    : AST_Expr(AST_TERNARY), cond(cond), v_true(vt), v_false(vf) {}

TreeNodeRef AST_Ternary::getTreeNode() const {
    auto node = TreeNode::create("?:"s);
    node->addChild(cond->getTreeNode());
    node->addChild(v_true->getTreeNode());
    node->addChild(v_false->getTreeNode());
    return node;
}


// AST_Assignment

AST_Assignment::AST_Assignment(OpType op, AST_Expr *lhs, AST_Expr *rhs)
    : AST_Expr(AST_ASSIGNMENT), op(op), lhs(lhs), rhs(rhs) {}

TreeNodeRef AST_Assignment::getTreeNode() const {
    auto node = TreeNode::create(opstr[op]);
    node->addChild(lhs->getTreeNode());
    node->addChild(rhs->getTreeNode());
    return node;
}

std::optional<AST_Binop::OpType> AST_Assignment::toBinop() const {
    switch (op) {
        case MUL:
            return AST_Binop::MUL;
        case DIV:
            return AST_Binop::DIV;
        case REM:
            return AST_Binop::REM;
        case ADD:
            return AST_Binop::ADD;
        case SUB:
            return AST_Binop::SUB;
        case SHL:
            return AST_Binop::SHL;
        case SHR:
            return AST_Binop::SHR;
        case AND:
            return AST_Binop::BIT_AND;
        case XOR:
            return AST_Binop::BIT_XOR;
        case OR:
            return AST_Binop::BIT_OR;
        default: // +DIRECT
            return {};
    }
}


// AST_CommaExpression

AST_CommaExpression::AST_CommaExpression(AST_Expr *expr)
        : AST_Expr(AST_COMMA_EXPR) {
    children.emplace_back(expr);
}

AST_CommaExpression* AST_CommaExpression::append(AST_Expr *expr) {
    children.emplace_back(expr);
    return this;
}

TreeNodeRef AST_CommaExpression::getTreeNode() const {
    auto node = TreeNode::create("comma"s);
    for (auto &e : children)
        node->addChild(e->getTreeNode());
    return node;
}


// =================================================
//                   Specifiers
// =================================================

// AST_TypeQualifiers

static void update_type_qualifiers(AST_TypeQualifiers &list, AST_TypeQualifiers::QualType qual) {
    switch (qual) {
        case AST_TypeQualifiers::Q_CONST:
            list.is_const = true;
            return;
        case AST_TypeQualifiers::Q_RESTRICT:
            list.is_restrict = true;
            return;
        case AST_TypeQualifiers::Q_VOLATILE:
            list.is_volatile = true;
            return;
        default:
            fmt::print(stderr, "Wrong qualifier: {}\n", qual);
            throw;
    }
}

AST_TypeQualifiers::AST_TypeQualifiers()
    : AST_Node(AST_TYPE_QUALIFIERS), is_const(false), is_restrict(false), is_volatile(false) {}

AST_TypeQualifiers::AST_TypeQualifiers(QualType init_qual)
    : AST_Node(AST_TYPE_QUALIFIERS), is_const(false), is_restrict(false), is_volatile(false)
{
    update_type_qualifiers(*this, init_qual);
}

AST_TypeQualifiers* AST_TypeQualifiers::update(QualType new_qual) {
    update_type_qualifiers(*this, new_qual);
    return this;
}

TreeNodeRef AST_TypeQualifiers::getTreeNode() const {
    std::string str("tq:"s);
    if (is_const)
        str += " const"s;
    if (is_restrict)
        str += " restrict"s;
    if (is_volatile)
        str += " volatile"s;
    return TreeNode::create(str);
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

TreeNodeRef AST_TypeSpecifier::getTreeNode() const {
    auto node = TreeNode::create(typestr[spec_type]);
    if (spec_type == T_UNISTRUCT || spec_type == T_ENUM)
        node->addChild(v->getTreeNode());
    // TODO: typename
    return node;
}


//AST_DeclSpecifiers

AST_DeclSpecifiers::AST_DeclSpecifiers()
    : AST_Node(AST_DECL_SPECIFIERS), type_qualifiers(new AST_TypeQualifiers()) {}

AST_DeclSpecifiers* AST_DeclSpecifiers::update_storage(ast_enum_t val) {
    if (storage_specifier != ST_NONE || val == ST_NONE) {
        fmt::print(stderr, "Wrong storage specifier: {}\n", storage_specifier);
        throw;
    }
    storage_specifier = StorageSpec(val);
    return this;
}

AST_DeclSpecifiers* AST_DeclSpecifiers::update_type_spec(AST_TypeSpecifier *val) {
    type_specifiers.emplace_back(val);
    return this;
}

AST_DeclSpecifiers* AST_DeclSpecifiers::update_type_qual(ast_enum_t val) {
    type_qualifiers->update(AST_TypeQualifiers::QualType(val));
    return this;
}

AST_DeclSpecifiers* AST_DeclSpecifiers::update_func_qual(ast_enum_t val) {
    if (val == AST_DeclSpecifiers::Q_INLINE)
        is_inline = true;
    else {
        fmt::print(stderr, "Wrong storage specifier: {}\n", storage_specifier);
        throw;
    }
    return this;
}

TreeNodeRef AST_DeclSpecifiers::getTreeNode() const {
    std::string str(storstr[storage_specifier]);
    if (is_inline)
        str += " inline"s;
    auto typeSpecNode = TreeNode::create("type_spec"s);
    for (auto &e : type_specifiers)
        typeSpecNode->addChild(e->getTreeNode());

    auto node = TreeNode::create(str);
    node->addChild(typeSpecNode);
    node->addChild(type_qualifiers->getTreeNode());
    return node;
}


// AST_StructDeclaration

AST_StructDeclaration::AST_StructDeclaration(AST_SpecifierQualifierList *type, AST_StructDeclaratorList *child)
    : AST_Node(AST_STRUCT_DECL), type(type), child(child) {}

TreeNodeRef AST_StructDeclaration::getTreeNode() const {
    auto node = TreeNode::create("struct_decl"s);
    node->addChild(type->getTreeNode());
    node->addChild(child->getTreeNode());
    return node;
}


// AST_StructDeclarator

AST_StructDeclarator::AST_StructDeclarator(AST_Declarator *decl, AST_Expr *width)
    : AST_Node(AST_STRUCT_DECLARATOR), declarator(decl), bitwidth(width) {}

TreeNodeRef AST_StructDeclarator::getTreeNode() const {
    auto node = TreeNode::create("struct_declarator"s);
    node->addChild(declarator->getTreeNode());
    if (bitwidth) {
        auto widthNode = TreeNode::create("bitwidth"s);
        widthNode->addChild(bitwidth->getTreeNode());
        node->addChild(widthNode);
    }
    return node;
}


// AST_StructDeclaratorList

AST_StructDeclaratorList::AST_StructDeclaratorList(AST_StructDeclarator *init) 
        : AST_Node(AST_STRUCT_DECLARATOR_LST) {
    children.emplace_back(init);
}

AST_StructDeclaratorList* AST_StructDeclaratorList::append(AST_StructDeclarator *decl) {
    children.emplace_back(decl);
    return this;
}

TreeNodeRef AST_StructDeclaratorList::getTreeNode() const {
    auto node = TreeNode::create("struct_decl_lst"s);
    for (auto &e : children)
        node->addChild(e->getTreeNode());
    return node;
}


// AST_StructDeclarationList

AST_StructDeclarationList::AST_StructDeclarationList(AST_StructDeclaration *init)
        : AST_Node(AST_STRUCT_DECL_LIST) {
    children.emplace_back(init);
}

AST_StructDeclarationList* AST_StructDeclarationList::append(AST_StructDeclaration *decl) {
    children.emplace_back(decl);
    return this;
}

TreeNodeRef AST_StructDeclarationList::getTreeNode() const {
    auto node = TreeNode::create("struct_decl_lst"s);
    for (auto &e : children)
        node->addChild(e->getTreeNode());
    return node;
}


// AST_SpecifierQualifierList

AST_SpecifierQualifierList::AST_SpecifierQualifierList(AST_TypeQualifiers::QualType qual) 
            : AST_Node(AST_SPEC_QUAL_LST), type_qualifiers(new AST_TypeQualifiers()) {
    type_qualifiers->update(qual);
}

AST_SpecifierQualifierList::AST_SpecifierQualifierList(AST_TypeSpecifier* type)
            : AST_Node(AST_SPEC_QUAL_LST), type_qualifiers(new AST_TypeQualifiers()) {
    type_specifiers.emplace_back(type);
}

AST_SpecifierQualifierList* AST_SpecifierQualifierList::append_qual(AST_TypeQualifiers::QualType qual) {
    type_qualifiers->update(qual);
    return this;
}

AST_SpecifierQualifierList* AST_SpecifierQualifierList::append_spec(AST_TypeSpecifier* type) {
    type_specifiers.emplace_back(type);
    return this;
}

TreeNodeRef AST_SpecifierQualifierList::getTreeNode() const {
    auto node = TreeNode::create("spec_qual_lst"s);
    if (type_qualifiers)
        node->addChild(type_qualifiers->getTreeNode());
    for (auto &e : type_specifiers)
        node->addChild(e->getTreeNode());
    return node;
}


// AST_StructOrUsionSpec

AST_StructOrUsionSpec::AST_StructOrUsionSpec(bool is_uni, string_id_t name, AST_StructDeclarationList *body)
    : AST_Node(AST_STRUCT_OR_UNION_SPEC), is_union(is_uni), name(name), body(body) {}

TreeNodeRef AST_StructOrUsionSpec::getTreeNode() const {
    std::string str(is_union ? "union"s : "struct"s);
    if (name != 0)
        str += "  "s + get_ident_by_id(name);
    auto node = TreeNode::create(str);
    if (body)
        node->addChild(body->getTreeNode());
    return node;
}


// AST_Enumerator

AST_Enumerator::AST_Enumerator(string_id_t name, AST_Expr *val)
    : AST_Node(AST_ENUMER), name(name), val(val) {}

TreeNodeRef AST_Enumerator::getTreeNode() const {
    auto node = TreeNode::create(get_ident_by_id(name));
    if (val)
        node->addChild(val->getTreeNode());
    return node;
}


// AST_EnumeratorList

AST_EnumeratorList::AST_EnumeratorList(AST_Enumerator *init)
        : AST_Node(AST_ENUMER_LST) {
    v.emplace_back(init);
}

AST_EnumeratorList* AST_EnumeratorList::append(AST_Enumerator *enumer) {
    v.emplace_back(enumer);
    return this;
}

TreeNodeRef AST_EnumeratorList::getTreeNode() const {
    auto node = TreeNode::create("enum_lst"s);
    for (auto &e : v)
        node->addChild(e->getTreeNode());
    return node;
}


// AST_EnumSpecifier

AST_EnumSpecifier::AST_EnumSpecifier(string_id_t name, AST_EnumeratorList *body)
    : AST_Node(AST_ENUM_SPEC), name(name), body(body) {}

TreeNodeRef AST_EnumSpecifier::getTreeNode() const {
    std::string str = "enum";
    if (name != 0)
        str += " "s + get_ident_by_id(name);
    auto node = TreeNode::create(str);
    node->addChild(body->getTreeNode());
    return node;
}


// =================================================
//                 Declarations
// =================================================

// AST_InitDeclarator

AST_InitDeclarator::AST_InitDeclarator(AST_Declarator *decl, AST_Initializer *init)
    : AST_Node(AST_INIT_DECL), declarator(decl), initializer(init) {}

TreeNodeRef AST_InitDeclarator::getTreeNode() const {
    auto node = TreeNode::create("init_decl"s);
    node->addChild(declarator->getTreeNode());
    if (initializer)
        node->addChild(initializer->getTreeNode());
    return node;
}


// AST_InitDeclaratorList

AST_InitDeclaratorList::AST_InitDeclaratorList(AST_InitDeclarator *init)
        : AST_Node(AST_INIT_DECL_LST) {
    v.emplace_back(init);
}

AST_InitDeclaratorList* AST_InitDeclaratorList::append(AST_InitDeclarator *decl) {
    v.emplace_back(decl);
    return this;
}

TreeNodeRef AST_InitDeclaratorList::getTreeNode() const {
    auto node = TreeNode::create("init_decl_lst"s);
    for (auto &e : v)
        node->addChild(e->getTreeNode());
    return node;
}


// AST_Declaration

AST_Declaration::AST_Declaration(AST_DeclSpecifiers *spec, AST_InitDeclaratorList *child)
    : AST_Node(AST_DECLARATION), specifiers(spec), child(child) {}

TreeNodeRef AST_Declaration::getTreeNode() const {
    auto node = TreeNode::create("declaration"s);
    node->addChild(specifiers->getTreeNode());
    if (child)
        node->addChild(child->getTreeNode());
    return node;
}


// AST_DirectDeclarator

AST_DirectDeclarator::AST_DirectDeclarator(DeclType dtype)
    : AST_Node(AST_DIR_DECLARATOR), type(dtype) {}

AST_DirectDeclarator* AST_DirectDeclarator::AST_DirectDeclarator::makeIdent(string_id_t ident) {
    auto res = new AST_DirectDeclarator(AST_DirectDeclarator::NAME);
    res->base = ident;
    return res;
}

AST_DirectDeclarator* AST_DirectDeclarator::AST_DirectDeclarator::makeNested(AST_Declarator *decl) {
    auto res = new AST_DirectDeclarator(AST_DirectDeclarator::NESTED);
    res->base = uniqify(decl);
    return res;
}

AST_DirectDeclarator* AST_DirectDeclarator::makeArr(AST_DirectDeclarator *base,
                                                    AST_TypeQualifiers *qual,
                                                    AST_Expr *sz) {
    auto res = new AST_DirectDeclarator(AST_DirectDeclarator::ARRAY);
    res->base = uniqify(base);
    res->arr_type_qual = uniqify(qual);
    res->arr_size = uniqify(sz);
    return res;
}

AST_DirectDeclarator* AST_DirectDeclarator::makeFunc(AST_DirectDeclarator *base,
                                                     AST_ParameterTypeList *args) {
    auto res = new AST_DirectDeclarator(AST_DirectDeclarator::FUNC);
    res->base = uniqify(base);
    res->func_args = uniqify(args);
    return res;
}

TreeNodeRef AST_DirectDeclarator::getTreeNode() const {
    if (type == AST_DirectDeclarator::NAME) {
        return TreeNode::create(get_ident_by_id(std::get<string_id_t>(base)));
    }
    else if (type == AST_DirectDeclarator::NESTED) {
        return std::get<uniq<AST_Node>>(base)->getTreeNode();
    }
    else if (type == AST_DirectDeclarator::ARRAY) {
        auto node = TreeNode::create("array_of"s);
        node->addChild(std::get<uniq<AST_Node>>(base)->getTreeNode());
        auto arr_node = TreeNode::create("properties"s);
        if (arr_type_qual)
            arr_node->addChild(arr_type_qual->getTreeNode());
        if (arr_size)
            arr_node->addChild(arr_size->getTreeNode());
        node->addChild(arr_node);
        return node;
    }
    else if (type == AST_DirectDeclarator::FUNC) {
        auto node = TreeNode::create("function"s);
        node->addChild(std::get<uniq<AST_Node>>(base)->getTreeNode());
        if (func_args)
            node->addChild(func_args->getTreeNode());
        else
            node->addChild(TreeNode::create("no_args"s));
        return node;
    }
    else {
        fmt::print(stderr, "Wrong direct declarator type: {}\n", type);
        throw;
    }
}

string_id_t AST_DirectDeclarator::getIdent() const {
    return std::get<string_id_t>(base);
}

AST_DirectDeclarator const &AST_DirectDeclarator::getBaseDirectDecl() const {
    auto const &uptr = std::get<std::unique_ptr<AST_Node>>(base);
    return dynamic_cast<AST_DirectDeclarator const &>(*uptr);
}

AST_Declarator const &AST_DirectDeclarator::getBaseDecl() const {
    auto const &uptr = std::get<std::unique_ptr<AST_Node>>(base);
    return dynamic_cast<AST_Declarator const &>(*uptr);
}


// AST_Pointer

AST_Pointer::AST_Pointer(AST_TypeQualifiers *qual, AST_Pointer *child)
    : AST_Node(AST_POINTER), qualifiers(qual), child(child) {}

TreeNodeRef AST_Pointer::getTreeNode() const {
    auto node = TreeNode::create("ptr_to"s);
    if (qualifiers)
        node->addChild(qualifiers->getTreeNode());
    if (child)
        node->addChild(child->getTreeNode());
    return node;
}


// AST_Declarator

AST_Declarator::AST_Declarator(AST_DirectDeclarator *decl, AST_Pointer *ptr)
    : AST_Node(AST_DECLARATOR), direct(decl), ptr(ptr) {}

TreeNodeRef AST_Declarator::getTreeNode() const {
    auto node = TreeNode::create("declarator"s);
    node->addChild(direct->getTreeNode());
    if (ptr)
        node->addChild(ptr->getTreeNode());
    return node;
}


// AST_ParameterDeclaration

AST_ParameterDeclaration::AST_ParameterDeclaration(AST_DeclSpecifiers *spec, AST_Declarator *child)
    : AST_Node(AST_PARAM_DECL), specifiers(spec), child(child){}


TreeNodeRef AST_ParameterDeclaration::getTreeNode() const {
    auto node = TreeNode::create("param_decl"s);
    node->addChild(specifiers->getTreeNode());
    if (child)
        node->addChild(child->getTreeNode());
    return node;
}


// AST_ParameterList

AST_ParameterList::AST_ParameterList(AST_ParameterDeclaration *init)
        : AST_Node(AST_PARAM_LST) {
    v.emplace_back(init);
}

AST_ParameterList* AST_ParameterList::append(AST_ParameterDeclaration *decl) {
    v.emplace_back(decl);
    return this;
}

TreeNodeRef AST_ParameterList::getTreeNode() const {
    auto node = TreeNode::create("param_lst"s);
    for (auto &e : v)
        node->addChild(e->getTreeNode());
    return node;
}


// AST_ParameterTypeList

AST_ParameterTypeList::AST_ParameterTypeList(AST_ParameterList *child, bool ellipsis)
    : AST_Node(AST_PARAM_TYPE_LST), v(child), has_ellipsis(ellipsis) {}

TreeNodeRef AST_ParameterTypeList::getTreeNode() const {
    std::string str("param_t_lst"s);
    if (has_ellipsis)
        str += "[...]"s;
    auto node = TreeNode::create(str);
    node->addChild(v->getTreeNode());
    return node;
}


// AST_TypeName

AST_TypeName::AST_TypeName(AST_SpecifierQualifierList *qual, AST_AbstractDeclarator *decl)
    : AST_Node(AST_TYPE_NAME), qual(qual), declarator(decl) {}

TreeNodeRef AST_TypeName::getTreeNode() const {
    auto node = TreeNode::create("type_name"s);
    node->addChild(qual->getTreeNode());
    if (declarator)
        node->addChild(declarator->getTreeNode());
    return node;
}


// AST_DirectAbstractDeclarator

AST_DirectAbstractDeclarator::AST_DirectAbstractDeclarator(DeclType dtype)
        : AST_Node(AST_DIR_ABSTRACT_DECL), type(dtype) {}

AST_DirectAbstractDeclarator* AST_DirectAbstractDeclarator::makeNested(
        AST_Node *decl) {
    auto res = new AST_DirectAbstractDeclarator(AST_DirectAbstractDeclarator::NESTED);
    res->base = uniqify(decl);
    return res;
}
AST_DirectAbstractDeclarator* AST_DirectAbstractDeclarator::makeArr(
        AST_Node *base, AST_Expr *sz) {
    auto res = new AST_DirectAbstractDeclarator(AST_DirectAbstractDeclarator::ARRAY);
    res->base = uniqify(base);
    res->arr_size = uniqify(sz);
    return res;
}
AST_DirectAbstractDeclarator* AST_DirectAbstractDeclarator::makeFunc(
        AST_Node *base, AST_ParameterTypeList *args) {
    auto res = new AST_DirectAbstractDeclarator(AST_DirectAbstractDeclarator::FUNC);
    res->base = uniqify(base);
    res->func_args = uniqify(args);
    return res;
}

TreeNodeRef AST_DirectAbstractDeclarator::getTreeNode() const {
    if (type == AST_DirectAbstractDeclarator::NESTED) {
        return base->getTreeNode();
    }
    else if (type == AST_DirectAbstractDeclarator::ARRAY) {
        auto node = TreeNode::create("abstr_array_of"s);
        if (base)
            node->addChild(base->getTreeNode());
        if (arr_size)
            node->addChild(arr_size->getTreeNode());
        return node;
    }
    else if (type == AST_DirectAbstractDeclarator::FUNC) {
        auto node = TreeNode::create("abstr_func"s);
        if (base)
            node->addChild(base->getTreeNode());
        if (func_args)
            node->addChild(func_args->getTreeNode());
        return node;
    }
    else {
        fmt::print(stderr, "Wrong direct abstract declarator type: {}\n", type);
        throw;
    }
}


// AST_AbstractDeclarator

AST_AbstractDeclarator::AST_AbstractDeclarator(
        AST_DirectAbstractDeclarator *decl, AST_Pointer *pointer)
        : AST_Node(AST_ABSTRACT_DECL), direct(decl), ptr(pointer) {}

TreeNodeRef AST_AbstractDeclarator::getTreeNode() const {
    auto node = TreeNode::create("abstr_decl"s);
    if (ptr)
        node->addChild(ptr->getTreeNode());
    if (direct)
        node->addChild(direct->getTreeNode());
    return node;
}


// =================================================
//                 Initializers
// =================================================

// AST_Designator

AST_Designator::AST_Designator(AST_Expr *val)
    : AST_Node(AST_DESIGNATOR), val(uniqify(val)), is_index(true) {}

AST_Designator::AST_Designator(string_id_t field)
    : AST_Node(AST_DESIGNATOR), val(field), is_index(false) {}


TreeNodeRef AST_Designator::getTreeNode() const {
    if (is_index) {
        auto node = TreeNode::create("designator[]"s);
        node->addChild(std::get<uniq<AST_Expr>>(val)->getTreeNode());
        return node;
    }
    else {
        auto node = TreeNode::create(
                "designator."s + get_ident_by_id(std::get<string_id_t>(val)));
        return node;
    }
}


// AST_InitializerList::AST_InitializerListElem

AST_InitializerList::AST_InitializerListElem::AST_InitializerListElem(
        AST_Initializer *v, AST_Designator *desig)
        : val(v), designator(desig) {}

TreeNodeRef AST_InitializerList::AST_InitializerListElem::getTreeNode() const {
    auto node = TreeNode::create("init"s);
    if (designator)
        node->addChild(designator->getTreeNode());
    node->addChild(val->getTreeNode());
    return node;
}


// AST_InitializerList

AST_InitializerList::AST_InitializerList(AST_Initializer *init_v, AST_Designator *init_desig)
    : AST_Node(AST_INITIALIZER_LST), children()
{
    children.emplace_back(init_v, init_desig);
}

AST_InitializerList* AST_InitializerList::append(AST_Initializer *val, AST_Designator *desig) {
    children.emplace_back(val, desig);
    return this;
}

TreeNodeRef AST_InitializerList::getTreeNode() const {
    auto node = TreeNode::create("init_lst"s);
    for (auto &e : children)
        node->addChild(e.getTreeNode());
    return node;
}


// AST_Initializer

AST_Initializer::AST_Initializer(AST_InitializerList *nest)
    : AST_Node(AST_INITIALIZER), is_compound(true), val(nest) {}

AST_Initializer::AST_Initializer(AST_Expr *val)
    : AST_Node(AST_INITIALIZER), is_compound(false), val(val) {}

TreeNodeRef AST_Initializer::getTreeNode() const {
    auto node = TreeNode::create(is_compound ? "compound_init"s : "direct_init"s);
    node->addChild(val->getTreeNode());
    return node;
}


// =================================================
//                   Statements
// =================================================

// AST_Statement

AST_Statement::AST_Statement(StmtType stype) : AST_Node(AST_STATEMENT), type(stype) {}


// AST_LabeledStmt

AST_LabeledStmt::AST_LabeledStmt(AST_Node *label, AST_Statement *stmt, LabelType type)
    : AST_Statement(AST_Statement::LABEL), label(uniqify(label)), child(stmt), type(type)
{
    if (type == AST_LabeledStmt::SIMPL)
        throw; // TODO: labeled statements
}

AST_LabeledStmt::AST_LabeledStmt(string_id_t label, AST_Statement *stmt, LabelType type)
    : AST_Statement(AST_Statement::LABEL), label(label), child(stmt), type(type) 
{
    if (type != AST_LabeledStmt::SIMPL)
        throw; // TODO: labeled statements
}

TreeNodeRef AST_LabeledStmt::getTreeNode() const {
    std::string str;
    if (type == AST_LabeledStmt::SIMPL)
        str = "label "s + get_ident_by_id(std::get<string_id_t>(label));
    else if (type == AST_LabeledStmt::SW_CASE)
        str = "case"s;
    else if (type == AST_LabeledStmt::SW_DEFAULT)
        str = "default"s;
    auto node = TreeNode::create(str);
    if (type == AST_LabeledStmt::SW_CASE)
        node->addChild(std::get<uniq<AST_Node>>(label)->getTreeNode());
    node->addChild(child->getTreeNode());
    return node;
}


// AST_BlockItemList

AST_BlockItemList::AST_BlockItemList() : AST_Node(AST_BLOCK_ITEM_LST), v() {}

AST_BlockItemList* AST_BlockItemList::append(AST_Node *child) {
    v.emplace_back(child);
    return this;
}

TreeNodeRef AST_BlockItemList::getTreeNode() const {
    auto node = TreeNode::create("block_item_lst"s);
    for (auto &e : v)
        node->addChild(e->getTreeNode());
    return node;
}


// AST_CompoundStmt

AST_CompoundStmt::AST_CompoundStmt(AST_BlockItemList *body)
    : AST_Statement(AST_Statement::COMPOUND), body(body) {}

TreeNodeRef AST_CompoundStmt::getTreeNode() const {
    auto node = TreeNode::create("compound_stmt"s);
    node->addChild(body->getTreeNode());
    return node;
}


// AST_ExprStmt

AST_ExprStmt::AST_ExprStmt(AST_Expr *child)
    : AST_Statement(AST_Statement::EXPR), child(child) {}

TreeNodeRef AST_ExprStmt::getTreeNode() const {
    auto node = TreeNode::create("expr_stmt"s);
    if (child)
        node->addChild(child->getTreeNode());
    return node;
}


// AST_SelectionStmt

AST_SelectionStmt::AST_SelectionStmt(bool sw)
    : AST_Statement(AST_Statement::SELECT), is_switch(sw) {}

AST_SelectionStmt* AST_SelectionStmt::get_if(AST_Expr *cond, AST_Statement *body,
                                             AST_Statement *else_body) {
    auto res = new AST_SelectionStmt(false);
    res->condition = uniqify(cond);
    res->body = uniqify(body);
    res->else_body = uniqify(else_body);
    return res;    
}

AST_SelectionStmt* AST_SelectionStmt::get_switch(AST_Expr *cond, AST_Statement *body) {
    auto res = new AST_SelectionStmt(true);
    res->condition = uniqify(cond);
    res->body = uniqify(body);
    res->else_body = nullptr;
    return res;
}

TreeNodeRef AST_SelectionStmt::getTreeNode() const {
    auto node = TreeNode::create(is_switch ? "switch"s : "if"s);
    node->addChild(condition->getTreeNode());
    if (!is_switch) {
        node->addChild(body->getTreeNode());
        if (else_body)
            node->addChild(else_body->getTreeNode());
    }
    else {
        node->addChild(body->getTreeNode());
    }
    return node;
}


// AST_IterationStmt

AST_IterationStmt::AST_IterationStmt(LoopType ltype, AST_Statement *body)
    : AST_Statement(AST_Statement::ITER), type(ltype), body(body) {}

std::unique_ptr<AST_Expr> const& AST_IterationStmt::getCond() const {
    if (type == AST_IterationStmt::FOR_LOOP)
        return std::get<ForLoopControls>(control).cond->child;
    else
        return std::get<std::unique_ptr<AST_Expr>>(control);
}

const AST_IterationStmt::ForLoopControls &AST_IterationStmt::getForLoopControls() const {
    return std::get<ForLoopControls>(control);
}

AST_IterationStmt* AST_IterationStmt::makeWhileLoop(AST_Statement *body, AST_Expr *ctl, bool is_do) {
    auto ltype = is_do ? AST_IterationStmt::DO_LOOP : AST_IterationStmt::WHILE_LOOP;
    auto res = new AST_IterationStmt(ltype, body);
    res->control = uniqify(ctl);
    return res;
}

AST_IterationStmt* AST_IterationStmt::makeForLoop(AST_Statement *body, AST_Node *decl,
                                                  AST_ExprStmt *cond, AST_Expr *act) {
    auto res = new AST_IterationStmt(AST_IterationStmt::FOR_LOOP, body);
    res->control = ForLoopControls{ uniqify(decl), uniqify(cond), uniqify(act) };
    return res;
}


TreeNodeRef AST_IterationStmt::ForLoopControls::getTreeNode() const {
    auto node = TreeNode::create("ctl"s);
    node->addChild(decl->getTreeNode());
    node->addChild(cond->getTreeNode());
    if (action)
        node->addChild(action->getTreeNode());
    return node;
}

TreeNodeRef AST_IterationStmt::getTreeNode() const {
    std::string str;
    TreeNodeRef ctl;
    if (type == AST_IterationStmt::FOR_LOOP) {
        str = "for"s;
        ctl = std::get<ForLoopControls>(control).getTreeNode();
    }
    else if (type == AST_IterationStmt::WHILE_LOOP) {
        str = "while"s;
        ctl = std::get<uniq<AST_Expr>>(control)->getTreeNode();
    }
    else if (type == AST_IterationStmt::DO_LOOP){
        str = "do while"s;
        ctl = std::get<uniq<AST_Expr>>(control)->getTreeNode();
    }
    auto node = TreeNode::create(str);
    node->addChild(ctl);
    node->addChild(body->getTreeNode());
    return node;
}


// AST_JumpStmt

AST_JumpStmt::AST_JumpStmt(JumpType jtype)
    : AST_Statement(AST_Statement::JUMP), type(jtype), arg(nullptr) {}

AST_JumpStmt::AST_JumpStmt(JumpType jtype, AST_Expr *arg)
    : AST_Statement(AST_Statement::JUMP), type(jtype), arg(uniqify(arg)) {}

AST_JumpStmt::AST_JumpStmt(JumpType jtype, string_id_t arg)
    : AST_Statement(AST_Statement::JUMP), type(jtype), arg(arg) {}

TreeNodeRef AST_JumpStmt::getTreeNode() const {
    std::string str;
    if (type == AST_JumpStmt::J_GOTO)
        str = "goto " + get_ident_by_id(std::get<string_id_t>(arg));
    else if (type == AST_JumpStmt::J_BREAK)
        str = "break"s;
    else if (type == AST_JumpStmt::J_CONTINUE)
        str = "continue"s;
    else if (type == AST_JumpStmt::J_RET)
        str = "return"s;
    auto node = TreeNode::create(str);
    if (std::get<uniq<AST_Expr>>(arg))
        node->addChild(std::get<uniq<AST_Expr>>(arg)->getTreeNode());
    return node;
}

std::unique_ptr<AST_Expr> const &AST_JumpStmt::getExpr() const {
    return std::get<std::unique_ptr<AST_Expr>>(arg);
}

string_id_t AST_JumpStmt::getIdent() const {
    return std::get<string_id_t>(arg);
}


// =================================================
//                Top-level elements
// =================================================

// AST_FunctionDef

AST_FunctionDef::AST_FunctionDef(AST_DeclSpecifiers *spec, AST_Declarator *decl, AST_CompoundStmt *body)
    : AST_Node(AST_FUNC_DEF), specifiers(spec), decl(decl), body(body) {}

TreeNodeRef AST_FunctionDef::getTreeNode() const {
    auto node = TreeNode::create("function_def"s);
    node->addChild(specifiers->getTreeNode());
    node->addChild(decl->getTreeNode());
    node->addChild(body->getTreeNode());
    return node;
}


// AST_TranslationUnit

AST_TranslationUnit::AST_TranslationUnit() : AST_Node(AST_TRANS_UNIT), children() {}

AST_TranslationUnit* AST_TranslationUnit::append(AST_Node *node) {
    children.emplace_back(node);
    return this;
}

TreeNodeRef AST_TranslationUnit::getTreeNode() const {
    auto node = TreeNode::create("t_unit"s);
    for (auto &e : children)
        node->addChild(e->getTreeNode());
    return node;
}
