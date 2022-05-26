#include "ast.hpp"
#include "core_driver.hpp"
#include <string>
#include <fmt/core.h>

using namespace std::string_literals;

AST_Node::AST_Node(int type) : node_type(type) {}

void AST_Node::setLoc(yy::location loc_) {
    this->loc = std::move(loc_);
}

// =================================================
//                    Expressions
// =================================================

AST_Expr::AST_Expr(int type) : AST_Node(type) {}

// AST_Primary

AST_Primary::AST_Primary(PrimType type)
    : AST_Expr(AST_PRIMARY), type(type) {}

AST_Primary* AbstractSyntaxTree::mkPrimIdent(string_id_t id, loc_t loc) {
    auto res = mkNode<AST_Primary>(std::move(loc), AST_Primary::IDENT);
    res->v = id;
    return res;
}

AST_Primary* AbstractSyntaxTree::mkPrimExpr(AST_Expr *expr, loc_t loc) {
    auto res = mkNode<AST_Primary>(std::move(loc), AST_Primary::EXPR);
    res->v = expr;
    return res;
}

AST_Primary* AbstractSyntaxTree::mkPrimStr(AST_StringsSeq *str, loc_t loc) {
    auto res = mkNode<AST_Primary>(std::move(loc), AST_Primary::STR);
    res->v = str;
    return res;
}

AST_Primary* AbstractSyntaxTree::mkPrimConst(AST_Literal val, loc_t loc) {
    auto res = mkNode<AST_Primary>(std::move(loc), AST_Primary::CONST);
    res->v = val;
    return res;
}

AST_Primary* AbstractSyntaxTree::mkPrimCompound(AST_TypeName *compType, AST_InitializerList *init_lst, loc_t loc) {
    auto res = mkNode<AST_Primary>(std::move(loc), AST_Primary::COMPOUND);
    res->v = AST_Primary::CompoundLiteral{
        .compType = compType,
        .val = init_lst,
    };
    return res;
}

string_id_t AST_Primary::getIdent() const {
    return std::get<string_id_t>(v);
}

AST_StringsSeq const& AST_Primary::getString() const {
    return *std::get<AST_StringsSeq*>(v);
}

AST_Literal AST_Primary::getLiteral() const {
    return std::get<AST_Literal>(v);
}

AST_Expr const& AST_Primary::getExpr() const {
    return *std::get<AST_Expr*>(v);
}

AST_Primary::CompoundLiteral const& AST_Primary::getCompound() const {
    return std::get<CompoundLiteral>(v);
}

TreeNodeRef AST_Primary::getTreeNode(ParsingContext const &pctx) const {
    if (type == AST_Primary::EXPR) {
        return std::get<AST_Expr*>(v)->getTreeNode(pctx);
    }
    else if (type == AST_Primary::STR) {
        return std::get<AST_StringsSeq*>(v)->getTreeNode(pctx);
    }
    else if (type == AST_Primary::COMPOUND) {
        throw cw39_not_implemented("Printing compound literals"); // TODO
    }
    else {
        std::string str;
        if (type == AST_Primary::IDENT)
            str = pctx.getIdentById(std::get<string_id_t>(v));
        else if (type == AST_Primary::CONST)
            str = "LITERAL"; // TODO
        return TreeNode::create(str);
    }
}


// AST_StringsSeq

AST_StringsSeq::AST_StringsSeq() : AST_Node(AST_STR_SEQ) {}

AST_StringsSeq* AbstractSyntaxTree::mkStringsSeq(loc_t loc) {
    return mkNode<AST_StringsSeq>(std::move(loc));
}

AST_StringsSeq* AST_StringsSeq::append(string_id_t str) {
    v.emplace_back(str);
    return this;
}

TreeNodeRef AST_StringsSeq::getTreeNode(ParsingContext const &pctx) const {
    if (v.size() == 1)
        return TreeNode::create("str["s + std::to_string(v.at(0)) + "]"s);
    auto node = TreeNode::create("concat");
    for (auto const &str : v)
        node->addChild(TreeNode::create("str["s + std::to_string(str) + "]"s));
    return node;
}


// AST_Postfix

AST_Postfix::AST_Postfix(OpType type)
    : AST_Expr(AST_POSTFIX), op(type) {}

AST_Postfix* AbstractSyntaxTree::mkPostfArr(AST_Expr *base, AST_Expr *size, loc_t loc) {
    auto res = mkNode<AST_Postfix>(std::move(loc), AST_Postfix::INDEXATION);
    res->base = base;
    res->arg = size;
    return res;
}

AST_Postfix* AbstractSyntaxTree::mkPostfCall(AST_Expr *base, AST_ArgumentsList *args, loc_t loc) {
    auto res = mkNode<AST_Postfix>(loc, AST_Postfix::CALL);
    res->base = base;
    if (args != nullptr)
        res->arg = args;
    else
        res->arg = mkArgsLst(loc); // Maybe, it is not exact location
    return res;
}

AST_Postfix* AbstractSyntaxTree::mkPostfAccesor(AST_Expr *base, string_id_t member, bool is_ptr, loc_t loc) {
    auto type = is_ptr ? AST_Postfix::PTR_ACCESS : AST_Postfix::DIR_ACCESS;
    auto res = mkNode<AST_Postfix>(std::move(loc), type);
    res->base = base;
    res->arg = member;
    return res;
}

AST_Postfix* AbstractSyntaxTree::mkPostfIncdec(AST_Expr *base, bool is_dec, loc_t loc) {
    auto type = is_dec ? AST_Postfix::POST_DEC : AST_Postfix::POST_INC;
    auto res = mkNode<AST_Postfix>(std::move(loc), type);
    res->base = base;
    return res;
}

TreeNodeRef AST_Postfix::getTreeNode(ParsingContext const &pctx) const {
    if (op == AST_Postfix::INDEXATION) {
        auto node = TreeNode::create("arr_at[]"s);
        node->addChild(base->getTreeNode(pctx));
        node->addChild(std::get<AST_Node*>(arg)->getTreeNode(pctx));
        return node;
    }
    else if (op == AST_Postfix::CALL) {
        auto node = TreeNode::create("call"s);
        node->addChild(base->getTreeNode(pctx));
        node->addChild(std::get<AST_Node*>(arg)->getTreeNode(pctx));
        return node;
    }

    std::string str;
    if (op == AST_Postfix::DIR_ACCESS)
        str = "."s + pctx.getIdentById(std::get<string_id_t>(arg));
    else if (op == AST_Postfix::PTR_ACCESS)
        str = "->"s + pctx.getIdentById(std::get<string_id_t>(arg));
    else if (op == AST_Postfix::POST_INC)
        str = "()++"s;
    else if (op == AST_Postfix::POST_DEC)
        str = "()--"s;
    auto node = TreeNode::create(str);
    node->addChild(base->getTreeNode(pctx));
    return node;
}

AST_Expr const& AST_Postfix::getExpr() const {
    auto const &uptr = std::get<AST_Node*>(arg);
    return dynamic_cast<AST_Expr const &>(*uptr);
}

AST_ArgumentsList const &AST_Postfix::getArgsList() const {
    auto const &uptr = std::get<AST_Node*>(arg);
    return dynamic_cast<AST_ArgumentsList const &>(*uptr);
}

string_id_t AST_Postfix::getIdent() const {
    return std::get<string_id_t>(arg);
}


// AST_ArgumentsList

AST_ArgumentsList::AST_ArgumentsList() : AST_Node(AST_ARGUMENTS_LIST) {}

AST_ArgumentsList* AbstractSyntaxTree::mkArgsLst(loc_t loc) {
    return mkNode<AST_ArgumentsList>(std::move(loc));
}

AST_ArgumentsList* AST_ArgumentsList::append(AST_Expr *arg) {
    children.emplace_back(arg);
    return this;
}

TreeNodeRef AST_ArgumentsList::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("args"s);
    for (auto &e : children)
        node->addChild(e->getTreeNode(pctx));
    return node;
}


// AST_Unop

AST_Unop::AST_Unop(OpType op, AST_Expr *child)
    : AST_Expr(AST_UNARY_OP), op(op), child(child) {}

AST_Unop::AST_Unop(OpType op, AST_TypeName *child)
    : AST_Expr(AST_UNARY_OP), op(op), child(child) {}

AST_Unop* AbstractSyntaxTree::mkUnop(AST_Unop::OpType op, AST_Expr *child, loc_t loc) {
    return mkNode<AST_Unop>(std::move(loc), op, child);
}

AST_Unop* AbstractSyntaxTree::mkUnop(AST_Unop::OpType op, AST_TypeName *child, loc_t loc) {
    return mkNode<AST_Unop>(std::move(loc), op, child);
}

TreeNodeRef AST_Unop::getTreeNode(ParsingContext const &pctx) const {
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
    node->addChild(child->getTreeNode(pctx));
    return node;
}


// AST_Cast

AST_Cast::AST_Cast(AST_TypeName *type, AST_Expr *child)
    : AST_Expr(AST_CAST), type_name(type), child(child) {}

AST_Cast* AbstractSyntaxTree::mkCastop(AST_TypeName *type, AST_Expr *child, loc_t loc) {
    return mkNode<AST_Cast>(std::move(loc), type, child);
}

TreeNodeRef AST_Cast::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("cast"s);
    node->addChild(type_name->getTreeNode(pctx));
    node->addChild(child->getTreeNode(pctx));
    return node;
}


// AST_Binop

AST_Binop::AST_Binop(OpType op, AST_Expr *lhs, AST_Expr *rhs)
    : AST_Expr(AST_BINOP), op(op), lhs(lhs), rhs(rhs) {}

AST_Binop* AbstractSyntaxTree::mkBinop(AST_Binop::OpType op, AST_Expr *lhs, AST_Expr *rhs, loc_t loc) {
    return mkNode<AST_Binop>(std::move(loc), op, lhs, rhs);
}

TreeNodeRef AST_Binop::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create(opstr[op]);
    node->addChild(lhs->getTreeNode(pctx));
    node->addChild(rhs->getTreeNode(pctx));
    return node;
}


// AST_Ternary

AST_Ternary::AST_Ternary(AST_Expr *cond, AST_Expr *vt, AST_Expr *vf)
    : AST_Expr(AST_TERNARY), cond(cond), v_true(vt), v_false(vf) {}

AST_Ternary* AbstractSyntaxTree::mkTernary(AST_Expr *cond, AST_Expr *vt, AST_Expr *vf, loc_t loc) {
    return mkNode<AST_Ternary>(std::move(loc), cond, vt, vf);
}

TreeNodeRef AST_Ternary::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("?:"s);
    node->addChild(cond->getTreeNode(pctx));
    node->addChild(v_true->getTreeNode(pctx));
    node->addChild(v_false->getTreeNode(pctx));
    return node;
}


// AST_Assignment

AST_Assignment::AST_Assignment(OpType op, AST_Expr *lhs, AST_Expr *rhs)
    : AST_Expr(AST_ASSIGNMENT), op(op), lhs(lhs), rhs(rhs) {}

AST_Assignment* AbstractSyntaxTree::mkAssign(AST_Assignment::OpType op, AST_Expr *lhs, AST_Expr *rhs, loc_t loc) {
    return mkNode<AST_Assignment>(std::move(loc), op, lhs, rhs);
}

TreeNodeRef AST_Assignment::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create(opstr[op]);
    node->addChild(lhs->getTreeNode(pctx));
    node->addChild(rhs->getTreeNode(pctx));
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

AST_CommaExpression::AST_CommaExpression(AST_Expr *expr1, AST_Expr *expr2)
        : AST_Expr(AST_COMMA_EXPR) {
    children.emplace_back(expr1);
    children.emplace_back(expr2);
}

AST_CommaExpression* AbstractSyntaxTree::mkCommaExpr(AST_Expr *expr1, AST_Expr *expr2, loc_t loc) {
    return mkNode<AST_CommaExpression>(std::move(loc), expr1, expr2);
}

TreeNodeRef AST_CommaExpression::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("comma"s);
    for (auto &e : children)
        node->addChild(e->getTreeNode(pctx));
    return node;
}


// =================================================
//                   Specifiers
// =================================================

// AST_TypeQualifiers

static void update_type_qualifiers(AST_TypeQuals &list, AST_TypeQuals::QualType qual) {
    switch (qual) {
        case AST_TypeQuals::Q_CONST:
            list.is_const = true;
            return;
        case AST_TypeQuals::Q_RESTRICT:
            list.is_restrict = true;
            return;
        case AST_TypeQuals::Q_VOLATILE:
            list.is_volatile = true;
            return;
        default:
            throw std::logic_error(fmt::format("Wrong type qualifier: {}", qual));
    }
}

AST_TypeQuals::AST_TypeQuals()
    : AST_Node(AST_TYPE_QUALIFIERS), is_const(false), is_restrict(false), is_volatile(false) {}

AST_TypeQuals::AST_TypeQuals(QualType init_qual)
    : AST_Node(AST_TYPE_QUALIFIERS), is_const(false), is_restrict(false), is_volatile(false)
{
    update_type_qualifiers(*this, init_qual);
}

AST_TypeQuals* AbstractSyntaxTree::mkTypeQuals(loc_t loc) {
    return mkNode<AST_TypeQuals>(std::move(loc));
}

AST_TypeQuals* AbstractSyntaxTree::mkTypeQuals(AST_TypeQuals::QualType init_qual, loc_t loc) {
    return mkNode<AST_TypeQuals>(std::move(loc), init_qual);
}

AST_TypeQuals* AST_TypeQuals::update(QualType new_qual) {
    update_type_qualifiers(*this, new_qual);
    return this;
}

TreeNodeRef AST_TypeQuals::getTreeNode(ParsingContext const &pctx) const {
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

AST_TypeSpecifier::AST_TypeSpecifier(AST_UStructSpec *spec)
    : AST_Node(AST_TYPE_SPECIFIER), spec_type(AST_TypeSpecifier::T_UNISTRUCT), v(spec) {}

AST_TypeSpecifier::AST_TypeSpecifier(AST_EnumSpecifier *spec)
    : AST_Node(AST_TYPE_SPECIFIER), spec_type(AST_TypeSpecifier::T_ENUM), v(spec) {}

AST_TypeSpecifier::AST_TypeSpecifier(AST_TypeName *spec)
    : AST_Node(AST_TYPE_SPECIFIER), spec_type(AST_TypeSpecifier::T_NAMED), v(spec) {}


AST_TypeSpecifier* AbstractSyntaxTree::mkTypeSpec(AST_TypeSpecifier::TypeSpec type, loc_t loc) {
    return mkNode<AST_TypeSpecifier>(std::move(loc), type);
}

AST_TypeSpecifier* AbstractSyntaxTree::mkTypeSpec(AST_UStructSpec *spec, loc_t loc) {
    return mkNode<AST_TypeSpecifier>(std::move(loc), spec);
}

AST_TypeSpecifier* AbstractSyntaxTree::mkTypeSpec(AST_TypeName *spec, loc_t loc) {
    return mkNode<AST_TypeSpecifier>(std::move(loc), spec);
}

AST_TypeSpecifier* AbstractSyntaxTree::mkTypeSpec(AST_EnumSpecifier *spec, loc_t loc) {
    return mkNode<AST_TypeSpecifier>(std::move(loc), spec);
}


TreeNodeRef AST_TypeSpecifier::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create(typestr[spec_type]);
    if (spec_type == T_UNISTRUCT || spec_type == T_ENUM)
        node->addChild(v->getTreeNode(pctx));
    // TODO: typename
    return node;
}


//AST_DeclSpecifiers

AST_DeclSpecifiers::AST_DeclSpecifiers(AST_TypeQuals *quals)
    : AST_Node(AST_DECL_SPECIFIERS), type_qualifiers(quals) {}

AST_DeclSpecifiers* AbstractSyntaxTree::mkDeclSpecs(loc_t loc) {
    return mkNode<AST_DeclSpecifiers>(loc, mkTypeQuals(loc));
}

AST_DeclSpecifiers* AST_DeclSpecifiers::update_storage(ast_enum_t val) {
    if (storage_specifier != ST_NONE || val == ST_NONE)
        throw std::logic_error(fmt::format("Wrong storage specifier: {}", storage_specifier));
    storage_specifier = StorageSpec(val);
    return this;
}

AST_DeclSpecifiers* AST_DeclSpecifiers::update_type_spec(AST_TypeSpecifier *val) {
    type_specifiers.emplace_back(val);
    return this;
}

AST_DeclSpecifiers* AST_DeclSpecifiers::update_type_qual(ast_enum_t val) {
    type_qualifiers->update(AST_TypeQuals::QualType(val));
    return this;
}

AST_DeclSpecifiers* AST_DeclSpecifiers::update_func_qual(ast_enum_t val) {
    if (val == AST_DeclSpecifiers::Q_INLINE)
        is_inline = true;
    else if (val == AST_DeclSpecifiers::Q_PURE)
        is_pure = true;
    else if (val == AST_DeclSpecifiers::Q_FCONST)
        is_fconst = true;
    else
        throw std::logic_error(fmt::format("Wrong function qualifier: {}", val));
    return this;
}

TreeNodeRef AST_DeclSpecifiers::getTreeNode(ParsingContext const &pctx) const {
    std::string str(storstr[storage_specifier]);
    if (is_inline)
        str += " inline"s;
    auto typeSpecNode = TreeNode::create("type_spec"s);
    for (auto &e : type_specifiers)
        typeSpecNode->addChild(e->getTreeNode(pctx));

    auto node = TreeNode::create(str);
    node->addChild(typeSpecNode);
    node->addChild(type_qualifiers->getTreeNode(pctx));
    return node;
}


// AST_StructDeclaration

AST_StructDeclaration::AST_StructDeclaration(AST_SpecsQualsList *type, AST_StructDeclaratorList *child)
    : AST_Node(AST_STRUCT_DECL), type(type), child(child) {}

AST_StructDeclaration*
AbstractSyntaxTree::mkStructDeclaration(AST_SpecsQualsList *type, AST_StructDeclaratorList *child, loc_t loc) {
    return mkNode<AST_StructDeclaration>(std::move(loc), type, child);
}

TreeNodeRef AST_StructDeclaration::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("struct_decl"s);
    node->addChild(type->getTreeNode(pctx));
    node->addChild(child->getTreeNode(pctx));
    return node;
}


// AST_StructDeclarator

AST_StructDeclarator::AST_StructDeclarator(AST_Declarator *decl, AST_Expr *width)
    : AST_Node(AST_STRUCT_DECLARATOR), declarator(decl), bitwidth(width) {}

AST_StructDeclarator* AbstractSyntaxTree::mkStructDeclarator(AST_Declarator *decl, AST_Expr *width, loc_t loc) {
    return mkNode<AST_StructDeclarator>(std::move(loc), decl, width);
}

TreeNodeRef AST_StructDeclarator::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("struct_declarator"s);
    node->addChild(declarator->getTreeNode(pctx));
    if (bitwidth) {
        auto widthNode = TreeNode::create("bitwidth"s);
        widthNode->addChild(bitwidth->getTreeNode(pctx));
        node->addChild(widthNode);
    }
    return node;
}


// AST_StructDeclaratorList

AST_StructDeclaratorList::AST_StructDeclaratorList(AST_StructDeclarator *init) 
        : AST_Node(AST_STRUCT_DECLARATOR_LST) {
    children.emplace_back(init);
}

AST_StructDeclaratorList* AbstractSyntaxTree::mkStructDeclaratorLst(AST_StructDeclarator *init, loc_t loc) {
    return mkNode<AST_StructDeclaratorList>(std::move(loc), init);
}

AST_StructDeclaratorList* AST_StructDeclaratorList::append(AST_StructDeclarator *decl) {
    children.emplace_back(decl);
    return this;
}

TreeNodeRef AST_StructDeclaratorList::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("struct_decl_lst"s);
    for (auto &e : children)
        node->addChild(e->getTreeNode(pctx));
    return node;
}


// AST_StructDeclarationList

AST_StructDeclarationList::AST_StructDeclarationList(AST_StructDeclaration *init)
        : AST_Node(AST_STRUCT_DECL_LIST) {
    children.emplace_back(init);
}

AST_StructDeclarationList* AbstractSyntaxTree::mkStructDeclarationLst(AST_StructDeclaration *init, loc_t loc) {
    return mkNode<AST_StructDeclarationList>(std::move(loc), init);
}

AST_StructDeclarationList* AST_StructDeclarationList::append(AST_StructDeclaration *decl) {
    children.emplace_back(decl);
    return this;
}

TreeNodeRef AST_StructDeclarationList::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("struct_decl_lst"s);
    for (auto &e : children)
        node->addChild(e->getTreeNode(pctx));
    return node;
}


// AST_SpecifierQualifierList

AST_SpecsQualsList::AST_SpecsQualsList(std::vector<AST_TypeSpecifier*> specs, AST_TypeQuals *quals)
            : AST_Node(AST_SPEC_QUAL_LST), type_specifiers(specs), type_qualifiers(quals) {}

AST_SpecsQualsList::AST_SpecsQualsList(AST_TypeQuals::QualType qual, AST_TypeQuals *quals)
            : AST_Node(AST_SPEC_QUAL_LST), type_qualifiers(quals) {
    type_qualifiers->update(qual);
}

AST_SpecsQualsList::AST_SpecsQualsList(AST_TypeSpecifier* type, AST_TypeQuals *quals)
            : AST_Node(AST_SPEC_QUAL_LST), type_qualifiers(quals) {
    type_specifiers.emplace_back(type);
}

AST_SpecsQualsList *AbstractSyntaxTree::mkSpecQualLst(std::vector<AST_TypeSpecifier*> specs, AST_TypeQuals *quals,
                                                      loc_t loc) {
    return mkNode<AST_SpecsQualsList>(std::move(loc), specs, quals);
}

AST_SpecsQualsList* AbstractSyntaxTree::mkSpecQualLst(AST_TypeQuals::QualType qual, loc_t loc) {
    return mkNode<AST_SpecsQualsList>(loc, qual, mkTypeQuals(loc));
}

AST_SpecsQualsList* AbstractSyntaxTree::mkSpecQualLst(AST_TypeSpecifier *type, loc_t loc) {
    return mkNode<AST_SpecsQualsList>(loc, type, mkTypeQuals(loc));
}

AST_SpecsQualsList* AST_SpecsQualsList::append_qual(AST_TypeQuals::QualType qual) {
    type_qualifiers->update(qual);
    return this;
}

AST_SpecsQualsList* AST_SpecsQualsList::append_spec(AST_TypeSpecifier* type) {
    type_specifiers.emplace_back(type);
    return this;
}

TreeNodeRef AST_SpecsQualsList::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("spec_qual_lst"s);
    if (type_qualifiers)
        node->addChild(type_qualifiers->getTreeNode(pctx));
    for (auto &e : type_specifiers)
        node->addChild(e->getTreeNode(pctx));
    return node;
}


// AST_StructOrUsionSpec

AST_UStructSpec::AST_UStructSpec(bool is_uni, string_id_t name, AST_StructDeclarationList *body)
    : AST_Node(AST_USTRUCT_SPEC), is_union(is_uni), name(name), body(body) {}

AST_UStructSpec* AbstractSyntaxTree::mkUstructSpec(bool is_uni, string_id_t name, AST_StructDeclarationList *body,
                                                   loc_t loc) {
    return mkNode<AST_UStructSpec>(std::move(loc), is_uni, name, body);
}

TreeNodeRef AST_UStructSpec::getTreeNode(ParsingContext const &pctx) const {
    std::string str(is_union ? "union"s : "struct"s);
    if (name != 0)
        str += "  "s + pctx.getIdentById(name);
    auto node = TreeNode::create(str);
    if (body)
        node->addChild(body->getTreeNode(pctx));
    return node;
}


// AST_Enumerator

AST_Enumerator::AST_Enumerator(string_id_t name, AST_Expr *val)
    : AST_Node(AST_ENUMER), name(name), val(val) {}

AST_Enumerator* AbstractSyntaxTree::mkEnumer(string_id_t name, AST_Expr *val, loc_t loc) {
    return mkNode<AST_Enumerator>(std::move(loc), name, val);
}

TreeNodeRef AST_Enumerator::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create(pctx.getIdentById(name));
    if (val)
        node->addChild(val->getTreeNode(pctx));
    return node;
}


// AST_EnumeratorList

AST_EnumeratorList::AST_EnumeratorList(AST_Enumerator *init)
        : AST_Node(AST_ENUMER_LST) {
    v.emplace_back(init);
}

AST_EnumeratorList* AbstractSyntaxTree::mkEnumLst(AST_Enumerator *init, loc_t loc) {
    return mkNode<AST_EnumeratorList>(std::move(loc), init);
}

AST_EnumeratorList* AST_EnumeratorList::append(AST_Enumerator *enumer) {
    v.emplace_back(enumer);
    return this;
}

TreeNodeRef AST_EnumeratorList::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("enum_lst"s);
    for (auto &e : v)
        node->addChild(e->getTreeNode(pctx));
    return node;
}


// AST_EnumSpecifier

AST_EnumSpecifier::AST_EnumSpecifier(string_id_t name, AST_EnumeratorList *body)
    : AST_Node(AST_ENUM_SPEC), name(name), body(body) {}

AST_EnumSpecifier* AbstractSyntaxTree::mkEnumSpec(string_id_t name, AST_EnumeratorList *body, loc_t loc) {
    return mkNode<AST_EnumSpecifier>(std::move(loc), name, body);
}

TreeNodeRef AST_EnumSpecifier::getTreeNode(ParsingContext const &pctx) const {
    std::string str = "enum";
    if (name != 0)
        str += " "s + pctx.getIdentById(name);
    auto node = TreeNode::create(str);
    node->addChild(body->getTreeNode(pctx));
    return node;
}


// =================================================
//                 Declarations
// =================================================

// AST_InitDeclarator

AST_InitDeclarator::AST_InitDeclarator(AST_Declarator *decl, AST_Initializer *init)
    : AST_Node(AST_INIT_DECL), declarator(decl), initializer(init) {}

AST_InitDeclarator* AbstractSyntaxTree::mkInitDeclarator(AST_Declarator *decl, AST_Initializer *init, loc_t loc) {
    return mkNode<AST_InitDeclarator>(std::move(loc), decl, init);
}

TreeNodeRef AST_InitDeclarator::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("init_decl"s);
    node->addChild(declarator->getTreeNode(pctx));
    if (initializer)
        node->addChild(initializer->getTreeNode(pctx));
    return node;
}


// AST_InitDeclaratorList

AST_InitDeclaratorList::AST_InitDeclaratorList(AST_InitDeclarator *init)
        : AST_Node(AST_INIT_DECL_LST) {
    v.emplace_back(init);
}

AST_InitDeclaratorList* AbstractSyntaxTree::mkInitDeclaratorLst(AST_InitDeclarator *init, loc_t loc) {
    return mkNode<AST_InitDeclaratorList>(std::move(loc), init);
}

AST_InitDeclaratorList* AST_InitDeclaratorList::append(AST_InitDeclarator *decl) {
    v.emplace_back(decl);
    return this;
}

TreeNodeRef AST_InitDeclaratorList::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("init_decl_lst"s);
    for (auto &e : v)
        node->addChild(e->getTreeNode(pctx));
    return node;
}


// AST_Declaration

AST_Declaration::AST_Declaration(AST_DeclSpecifiers *spec, AST_InitDeclaratorList *child)
    : AST_Node(AST_DECLARATION), specifiers(spec), child(child) {}

AST_Declaration* AbstractSyntaxTree::mkDeclaration(AST_DeclSpecifiers *spec, AST_InitDeclaratorList *child, loc_t loc) {
    return mkNode<AST_Declaration>(std::move(loc), spec, child);
}

TreeNodeRef AST_Declaration::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("declaration"s);
    node->addChild(specifiers->getTreeNode(pctx));
    if (child)
        node->addChild(child->getTreeNode(pctx));
    return node;
}


// AST_DirectDeclarator

AST_DirDeclarator::AST_DirDeclarator(DeclType dtype)
    : AST_Node(AST_DIR_DECLARATOR), type(dtype) {}

AST_DirDeclarator* AbstractSyntaxTree::mkDirDeclIdent(string_id_t ident, loc_t loc) {
    auto res = mkNode<AST_DirDeclarator>(std::move(loc), AST_DirDeclarator::NAME);
    res->base = ident;
    return res;
}

AST_DirDeclarator* AbstractSyntaxTree::mkDirDeclNested(AST_Declarator *decl, loc_t loc) {
    auto res = mkNode<AST_DirDeclarator>(std::move(loc), AST_DirDeclarator::NESTED);
    res->base = decl;
    return res;
}

AST_DirDeclarator* AbstractSyntaxTree::mkDirDeclArr(AST_DirDeclarator *base, AST_TypeQuals *qual,
                                                    AST_Expr *sz, bool isStatic, loc_t loc) {
    auto res = mkNode<AST_DirDeclarator>(std::move(loc), AST_DirDeclarator::ARRAY);
    res->base = base;
    res->arr_type_qual = qual;
    res->arr_size = sz;
    res->isStatic = isStatic;
    return res;
}

AST_DirDeclarator* AbstractSyntaxTree::mkDirDeclFunc(AST_DirDeclarator *base, AST_ParameterTypeList *args, loc_t loc) {
    auto res = mkNode<AST_DirDeclarator>(std::move(loc), AST_DirDeclarator::FUNC);
    res->base = base;
    res->func_args = args;
    return res;
}

TreeNodeRef AST_DirDeclarator::getTreeNode(ParsingContext const &pctx) const {
    if (type == AST_DirDeclarator::NAME) {
        return TreeNode::create(pctx.getIdentById(std::get<string_id_t>(base)));
    }
    else if (type == AST_DirDeclarator::NESTED) {
        return std::get<AST_Node*>(base)->getTreeNode(pctx);
    }
    else if (type == AST_DirDeclarator::ARRAY) {
        auto node = TreeNode::create("array_of"s);
        node->addChild(std::get<AST_Node*>(base)->getTreeNode(pctx));
        auto arr_node = TreeNode::create("properties"s);
        if (arr_type_qual)
            arr_node->addChild(arr_type_qual->getTreeNode(pctx));
        if (arr_size)
            arr_node->addChild(arr_size->getTreeNode(pctx));
        node->addChild(arr_node);
        return node;
    }
    else if (type == AST_DirDeclarator::FUNC) {
        auto node = TreeNode::create("function"s);
        node->addChild(std::get<AST_Node*>(base)->getTreeNode(pctx));
        if (func_args)
            node->addChild(func_args->getTreeNode(pctx));
        else
            node->addChild(TreeNode::create("no_args"s));
        return node;
    }
    else {
        throw std::logic_error(fmt::format("Wrong direct declarator type: {}", type));
    }
}

string_id_t AST_DirDeclarator::getIdent() const {
    return std::get<string_id_t>(base);
}

AST_DirDeclarator const &AST_DirDeclarator::getBaseDirectDecl() const {
    auto const &uptr = std::get<AST_Node*>(base);
    return dynamic_cast<AST_DirDeclarator const &>(*uptr);
}

AST_Declarator const &AST_DirDeclarator::getBaseDecl() const {
    auto const &uptr = std::get<AST_Node*>(base);
    return dynamic_cast<AST_Declarator const &>(*uptr);
}


// AST_Pointer

AST_Pointer::AST_Pointer(AST_TypeQuals *qual, AST_Pointer *child)
    : AST_Node(AST_POINTER), qualifiers(qual), child(child) {}

AST_Pointer* AbstractSyntaxTree::mkPointer(AST_TypeQuals *qual, AST_Pointer *child, loc_t loc) {
    return mkNode<AST_Pointer>(std::move(loc), qual, child);
}

TreeNodeRef AST_Pointer::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("ptr_to"s);
    if (qualifiers)
        node->addChild(qualifiers->getTreeNode(pctx));
    if (child)
        node->addChild(child->getTreeNode(pctx));
    return node;
}


// AST_Declarator

AST_Declarator::AST_Declarator(AST_DirDeclarator *decl, AST_Pointer *ptr)
    : AST_Node(AST_DECLARATOR), direct(decl), ptr(ptr) {}

AST_Declarator* AbstractSyntaxTree::mkDeclarator(AST_DirDeclarator *decl, AST_Pointer *ptr, loc_t loc) {
    return mkNode<AST_Declarator>(std::move(loc), decl, ptr);
}

AST_ParameterDeclaration* AbstractSyntaxTree::mkParamDecl(AST_DeclSpecifiers *spec, AST_Declarator *child, loc_t loc) {
    return mkNode<AST_ParameterDeclaration>(std::move(loc), spec, child);
}

TreeNodeRef AST_Declarator::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("declarator"s);
    node->addChild(direct->getTreeNode(pctx));
    if (ptr)
        node->addChild(ptr->getTreeNode(pctx));
    return node;
}


// AST_ParameterDeclaration

AST_ParameterDeclaration::AST_ParameterDeclaration(AST_DeclSpecifiers *spec, AST_Declarator *child)
    : AST_Node(AST_PARAM_DECL), specifiers(spec), child(child){}


TreeNodeRef AST_ParameterDeclaration::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("param_decl"s);
    node->addChild(specifiers->getTreeNode(pctx));
    if (child)
        node->addChild(child->getTreeNode(pctx));
    return node;
}


// AST_ParameterList

AST_ParameterList::AST_ParameterList(AST_ParameterDeclaration *init)
        : AST_Node(AST_PARAM_LST) {
    v.emplace_back(init);
}

AST_ParameterList* AbstractSyntaxTree::mkParamLst(AST_ParameterDeclaration *init, loc_t loc) {
    return mkNode<AST_ParameterList>(std::move(loc), init);
}

AST_ParameterList* AST_ParameterList::append(AST_ParameterDeclaration *decl) {
    v.emplace_back(decl);
    return this;
}

TreeNodeRef AST_ParameterList::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("param_lst"s);
    for (auto &e : v)
        node->addChild(e->getTreeNode(pctx));
    return node;
}


// AST_ParameterTypeList

AST_ParameterTypeList::AST_ParameterTypeList(AST_ParameterList *child, bool ellipsis)
    : AST_Node(AST_PARAM_TYPE_LST), v(child), has_ellipsis(ellipsis) {}

AST_ParameterTypeList* AbstractSyntaxTree::mkParamTypeLst(AST_ParameterList *child, bool ellipsis, loc_t loc) {
    return mkNode<AST_ParameterTypeList>(std::move(loc), child, ellipsis);
}

TreeNodeRef AST_ParameterTypeList::getTreeNode(ParsingContext const &pctx) const {
    std::string str("param_t_lst"s);
    if (has_ellipsis)
        str += "[...]"s;
    auto node = TreeNode::create(str);
    node->addChild(v->getTreeNode(pctx));
    return node;
}


// AST_TypeName

AST_TypeName::AST_TypeName(AST_SpecsQualsList *qual, AST_AbstrDeclarator *decl)
    : AST_Node(AST_TYPE_NAME), qual(qual), declarator(decl ? decl : optDeclType()) {}

AST_TypeName::AST_TypeName(AST_SpecsQualsList *qual, AST_Declarator *decl)
    : AST_Node(AST_TYPE_NAME), qual(qual), declarator(decl ? decl : optDeclType()) {}

AST_TypeName* AbstractSyntaxTree::mkTypeName(AST_SpecsQualsList *qual, std::nullptr_t decl, loc_t loc) {
    return mkNode<AST_TypeName>(std::move(loc), qual, static_cast<AST_AbstrDeclarator*>(decl));
}

AST_TypeName* AbstractSyntaxTree::mkTypeName(AST_SpecsQualsList *qual, AST_AbstrDeclarator *decl, loc_t loc) {
    return mkNode<AST_TypeName>(std::move(loc), qual, decl);
}

AST_TypeName *AbstractSyntaxTree::mkTypeName(AST_SpecsQualsList *qual, AST_Declarator *decl, loc_t loc) {
    return mkNode<AST_TypeName>(std::move(loc), qual, decl);
}

TreeNodeRef AST_TypeName::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("type_name"s);
    node->addChild(qual->getTreeNode(pctx));
    if (declarator) {
        std::visit([&node, &pctx](auto const &decl) {
            node->addChild(decl->getTreeNode(pctx));
        }, *declarator);
    }
    return node;
}


// AST_DirectAbstractDeclarator

AST_DirAbstrDeclarator::AST_DirAbstrDeclarator(DeclType dtype)
        : AST_Node(AST_DIR_ABSTRACT_DECL), type(dtype) {}

AST_DirAbstrDeclarator* AbstractSyntaxTree::mkDirAbstrDeclNested(AST_Node *decl, loc_t loc) {
    auto res = mkNode<AST_DirAbstrDeclarator>(std::move(loc), AST_DirAbstrDeclarator::NESTED);
    res->base = decl;
    return res;
}

AST_DirAbstrDeclarator* AbstractSyntaxTree::mkDirAbstrDeclArr(AST_Node *base, AST_Expr *sz, loc_t loc) {
    auto res = mkNode<AST_DirAbstrDeclarator>(std::move(loc), AST_DirAbstrDeclarator::ARRAY);
    res->base = base;
    res->arr_size = sz;
    return res;
}

AST_DirAbstrDeclarator* AbstractSyntaxTree::mkDirAbstrDeclFunc(AST_Node *base, AST_ParameterTypeList *args, loc_t loc) {
    auto res = mkNode<AST_DirAbstrDeclarator>(std::move(loc), AST_DirAbstrDeclarator::FUNC);
    res->base = base;
    res->func_args = args;
    return res;
}

TreeNodeRef AST_DirAbstrDeclarator::getTreeNode(ParsingContext const &pctx) const {
    if (type == AST_DirAbstrDeclarator::NESTED) {
        return base->getTreeNode(pctx);
    }
    else if (type == AST_DirAbstrDeclarator::ARRAY) {
        auto node = TreeNode::create("abstr_array_of"s);
        if (base)
            node->addChild(base->getTreeNode(pctx));
        if (arr_size)
            node->addChild(arr_size->getTreeNode(pctx));
        return node;
    }
    else if (type == AST_DirAbstrDeclarator::FUNC) {
        auto node = TreeNode::create("abstr_func"s);
        if (base)
            node->addChild(base->getTreeNode(pctx));
        if (func_args)
            node->addChild(func_args->getTreeNode(pctx));
        return node;
    }
    else {
        throw std::logic_error(fmt::format("Wrong direct abstract declarator type: {}", type));
    }
}

AST_DirAbstrDeclarator const &AST_DirAbstrDeclarator::getBaseDirectDecl() const {
    return dynamic_cast<AST_DirAbstrDeclarator const &>(*base);
}

AST_AbstrDeclarator const &AST_DirAbstrDeclarator::getBaseDecl() const {
    return dynamic_cast<AST_AbstrDeclarator const &>(*base);
}


// AST_AbstractDeclarator

AST_AbstrDeclarator::AST_AbstrDeclarator(AST_DirAbstrDeclarator *decl, AST_Pointer *pointer)
        : AST_Node(AST_ABSTRACT_DECL), direct(decl), ptr(pointer) {}

AST_AbstrDeclarator* AbstractSyntaxTree::mkAbstrDeclarator(AST_DirAbstrDeclarator *decl, AST_Pointer *pointer,
                                                           loc_t loc) {
    return mkNode<AST_AbstrDeclarator>(std::move(loc), decl, pointer);
}

TreeNodeRef AST_AbstrDeclarator::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("abstr_decl"s);
    if (ptr)
        node->addChild(ptr->getTreeNode(pctx));
    if (direct)
        node->addChild(direct->getTreeNode(pctx));
    return node;
}


// =================================================
//                 Initializers
// =================================================

// AST_Designator

AST_Designator::AST_Designator(AST_Expr *val)
    : AST_Node(AST_DESIGNATOR), val(val), is_index(true) {}

AST_Designator::AST_Designator(string_id_t field)
    : AST_Node(AST_DESIGNATOR), val(field), is_index(false) {}

AST_Designator* AbstractSyntaxTree::mkDesignator(string_id_t field, loc_t loc) {
    return mkNode<AST_Designator>(std::move(loc), field);
}

AST_Designator* AbstractSyntaxTree::mkDesignator(AST_Expr *val, loc_t loc) {
    return mkNode<AST_Designator>(std::move(loc), val);
}

TreeNodeRef AST_Designator::getTreeNode(ParsingContext const &pctx) const {
    if (is_index) {
        auto node = TreeNode::create("designator[]"s);
        node->addChild(std::get<AST_Expr*>(val)->getTreeNode(pctx));
        return node;
    }
    else {
        auto node = TreeNode::create(
                "designator."s + pctx.getIdentById(std::get<string_id_t>(val)));
        return node;
    }
}


// AST_InitializerList::AST_InitializerListElem

AST_InitializerList::AST_InitializerListElem::AST_InitializerListElem(
        AST_Initializer *v, AST_Designator *desig)
        : val(v), designator(desig) {}

TreeNodeRef AST_InitializerList::AST_InitializerListElem::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("init"s);
    if (designator)
        node->addChild(designator->getTreeNode(pctx));
    node->addChild(val->getTreeNode(pctx));
    return node;
}


// AST_InitializerList

AST_InitializerList::AST_InitializerList(AST_Initializer *init_v, AST_Designator *init_desig)
        : AST_Node(AST_INITIALIZER_LST), children()
{
    children.emplace_back(init_v, init_desig);
}

AST_InitializerList* AbstractSyntaxTree::mkInitializerLst(AST_Initializer *init_v, AST_Designator *init_desig,
                                                          loc_t loc) {
    return mkNode<AST_InitializerList>(std::move(loc), init_v, init_desig);
}

AST_InitializerList* AST_InitializerList::append(AST_Initializer *val, AST_Designator *desig) {
    children.emplace_back(val, desig);
    return this;
}

TreeNodeRef AST_InitializerList::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("init_lst"s);
    for (auto &e : children)
        node->addChild(e.getTreeNode(pctx));
    return node;
}


// AST_Initializer

AST_Initializer::AST_Initializer(AST_InitializerList *nest)
    : AST_Node(AST_INITIALIZER), is_compound(true), val(nest) {}

AST_Initializer::AST_Initializer(AST_Expr *val)
    : AST_Node(AST_INITIALIZER), is_compound(false), val(val) {}

AST_Initializer* AbstractSyntaxTree::mkInitializer(AST_InitializerList *nest, loc_t loc) {
    return mkNode<AST_Initializer>(std::move(loc), nest);
}

AST_Initializer* AbstractSyntaxTree::mkInitializer(AST_Expr *val, loc_t loc) {
    return mkNode<AST_Initializer>(std::move(loc), val);
}

AST_Expr const &AST_Initializer::getExpr() const {
    return dynamic_cast<AST_Expr const &>(*val);
}

AST_InitializerList const &AST_Initializer::getInitList() const {
    return dynamic_cast<AST_InitializerList const &>(*val);
}

TreeNodeRef AST_Initializer::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create(is_compound ? "compound_init"s : "direct_init"s);
    node->addChild(val->getTreeNode(pctx));
    return node;
}


// =================================================
//                   Statements
// =================================================

// AST_Statement

AST_Stmt::AST_Stmt(StmtType stype) : AST_Node(AST_STATEMENT), type(stype) {}


// AST_LabeledStmt

AST_LabeledStmt::AST_LabeledStmt(AST_Expr *label, AST_Stmt *stmt, LabelType type)
      : AST_Stmt(AST_Stmt::LABEL), label(label), child(stmt), type(type)
{
    if (type == AST_LabeledStmt::SIMPL)
        throw parser_exception("Wrong type for simple label", "");
    else if (type == AST_LabeledStmt::SW_CASE && !label)
        throw parser_exception("'case' label without argument", "");
    else if (type == AST_LabeledStmt::SW_DEFAULT && label)
        throw parser_exception("'default' label with argument", "");
}

AST_LabeledStmt::AST_LabeledStmt(string_id_t label, AST_Stmt *stmt, LabelType type)
      : AST_Stmt(AST_Stmt::LABEL), label(label), child(stmt), type(type)
{
    if (type != AST_LabeledStmt::SIMPL)
        throw parser_exception("Wrong type for switch label", "");
}

AST_LabeledStmt* AbstractSyntaxTree::mkLabelStmt(AST_Expr *label, AST_Stmt *stmt, AST_LabeledStmt::LabelType type,
                                                 loc_t loc) {
    return mkNode<AST_LabeledStmt>(std::move(loc), label, stmt, type);
}

AST_LabeledStmt* AbstractSyntaxTree::mkLabelStmt(string_id_t label, AST_Stmt *stmt, AST_LabeledStmt::LabelType type,
                                                 loc_t loc) {
    return mkNode<AST_LabeledStmt>(std::move(loc), label, stmt, type);
}

TreeNodeRef AST_LabeledStmt::getTreeNode(ParsingContext const &pctx) const {
    std::string str;
    if (type == AST_LabeledStmt::SIMPL)
        str = "label "s + pctx.getIdentById(getIdent());
    else if (type == AST_LabeledStmt::SW_CASE)
        str = "case"s;
    else if (type == AST_LabeledStmt::SW_DEFAULT)
        str = "default"s;
    auto node = TreeNode::create(str);
    if (type == AST_LabeledStmt::SW_CASE)
        node->addChild(getExpr().getTreeNode(pctx));
    node->addChild(child->getTreeNode(pctx));
    return node;
}

string_id_t AST_LabeledStmt::getIdent() const {
    return std::get<string_id_t>(label);
}

AST_Expr const& AST_LabeledStmt::getExpr() const {
    return *std::get<AST_Expr*>(label);
}


// AST_BlockItemList

AST_BlockItemList::AST_BlockItemList() : AST_Node(AST_BLOCK_ITEM_LST), v() {}

AST_BlockItemList* AbstractSyntaxTree::mkBlockItemLst(loc_t loc) {
    return mkNode<AST_BlockItemList>(std::move(loc));
}

AST_BlockItemList* AST_BlockItemList::append(AST_Node *child) {
    v.emplace_back(child);
    return this;
}

TreeNodeRef AST_BlockItemList::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("block_item_lst"s);
    for (auto &e : v)
        node->addChild(e->getTreeNode(pctx));
    return node;
}


// AST_CompoundStmt

AST_CompoundStmt::AST_CompoundStmt(AST_BlockItemList *body)
    : AST_Stmt(AST_Stmt::COMPOUND), body(body) {}

AST_CompoundStmt* AbstractSyntaxTree::mkCompoundStmt(AST_BlockItemList *body, loc_t loc) {
    return mkNode<AST_CompoundStmt>(std::move(loc), body);
}

TreeNodeRef AST_CompoundStmt::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("compound_stmt"s);
    node->addChild(body->getTreeNode(pctx));
    return node;
}


// AST_ExprStmt

AST_ExprStmt::AST_ExprStmt(AST_Expr *child)
    : AST_Stmt(AST_Stmt::EXPR), child(child) {}

AST_ExprStmt* AbstractSyntaxTree::mkExprStmt(AST_Expr *child, loc_t loc) {
    return mkNode<AST_ExprStmt>(std::move(loc), child);
}

TreeNodeRef AST_ExprStmt::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("expr_stmt"s);
    if (child)
        node->addChild(child->getTreeNode(pctx));
    return node;
}


// AST_SelectionStmt

AST_SelectionStmt::AST_SelectionStmt(bool sw)
    : AST_Stmt(AST_Stmt::SELECT), is_switch(sw) {}

AST_SelectionStmt* AbstractSyntaxTree::mkIfStmt(AST_Expr *cond, AST_Stmt *body, AST_Stmt *else_body, loc_t loc) {
    auto res = mkNode<AST_SelectionStmt>(std::move(loc), false);
    res->condition = cond;
    res->body = body;
    res->else_body = else_body;
    return res;
}

AST_SelectionStmt* AbstractSyntaxTree::mkSwitchStmt(AST_Expr *cond, AST_Stmt *body, loc_t loc) {
    auto res = mkNode<AST_SelectionStmt>(std::move(loc), true);
    res->condition = cond;
    res->body = body;
    res->else_body = nullptr;
    return res;
}

TreeNodeRef AST_SelectionStmt::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create(is_switch ? "switch"s : "if"s);
    node->addChild(condition->getTreeNode(pctx));
    if (!is_switch) {
        node->addChild(body->getTreeNode(pctx));
        if (else_body)
            node->addChild(else_body->getTreeNode(pctx));
    }
    else {
        node->addChild(body->getTreeNode(pctx));
    }
    return node;
}


// AST_IterationStmt

AST_IterStmt::AST_IterStmt(LoopType ltype, AST_Stmt *body)
    : AST_Stmt(AST_Stmt::ITER), type(ltype), body(body) {}

AST_Expr* AST_IterStmt::getCond() const {
    if (type == AST_IterStmt::FOR_LOOP)
        return std::get<ForLoopControls>(control).cond->child;
    else
        return std::get<AST_Expr*>(control);
}

AST_IterStmt::ForLoopControls const& AST_IterStmt::getForLoopControls() const {
    return std::get<ForLoopControls>(control);
}

AST_IterStmt* AbstractSyntaxTree::makeWhileStmt(AST_Stmt *body, AST_Expr *ctl, bool is_do, loc_t loc) {
    auto ltype = is_do ? AST_IterStmt::DO_LOOP : AST_IterStmt::WHILE_LOOP;
    auto res = mkNode<AST_IterStmt>(std::move(loc), ltype, body);
    res->control = ctl;
    return res;
}

AST_IterStmt* AbstractSyntaxTree::makeForStmt(AST_Stmt *body, AST_Node *decl, AST_ExprStmt *cond,
                                              AST_Expr *act, loc_t loc) {
    auto res = mkNode<AST_IterStmt>(std::move(loc), AST_IterStmt::FOR_LOOP, body);
    res->control = AST_IterStmt::ForLoopControls{ decl, cond, act };
    return res;
}


TreeNodeRef AST_IterStmt::ForLoopControls::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("ctl"s);
    node->addChild(decl->getTreeNode(pctx));
    node->addChild(cond->getTreeNode(pctx));
    if (action)
        node->addChild(action->getTreeNode(pctx));
    return node;
}

TreeNodeRef AST_IterStmt::getTreeNode(ParsingContext const &pctx) const {
    std::string str;
    TreeNodeRef ctl;
    if (type == AST_IterStmt::FOR_LOOP) {
        str = "for"s;
        ctl = std::get<ForLoopControls>(control).getTreeNode(pctx);
    }
    else if (type == AST_IterStmt::WHILE_LOOP) {
        str = "while"s;
        ctl = std::get<AST_Expr*>(control)->getTreeNode(pctx);
    }
    else if (type == AST_IterStmt::DO_LOOP){
        str = "do while"s;
        ctl = std::get<AST_Expr*>(control)->getTreeNode(pctx);
    }
    auto node = TreeNode::create(str);
    node->addChild(ctl);
    node->addChild(body->getTreeNode(pctx));
    return node;
}


// AST_JumpStmt

AST_JumpStmt::AST_JumpStmt(JumpType jtype)
    : AST_Stmt(AST_Stmt::JUMP), type(jtype), arg(nullptr) {}

AST_JumpStmt::AST_JumpStmt(JumpType jtype, AST_Expr *arg)
    : AST_Stmt(AST_Stmt::JUMP), type(jtype), arg(arg) {}

AST_JumpStmt::AST_JumpStmt(JumpType jtype, string_id_t arg)
    : AST_Stmt(AST_Stmt::JUMP), type(jtype), arg(arg) {}

AST_JumpStmt* AbstractSyntaxTree::mkJumpStmt(AST_JumpStmt::JumpType jtype, loc_t loc) {
    return mkNode<AST_JumpStmt>(std::move(loc), jtype);
}

AST_JumpStmt* AbstractSyntaxTree::mkJumpStmt(AST_JumpStmt::JumpType jtype, AST_Expr *arg, loc_t loc) {
    return mkNode<AST_JumpStmt>(std::move(loc), jtype, arg);
}

AST_JumpStmt* AbstractSyntaxTree::mkJumpStmt(AST_JumpStmt::JumpType jtype, string_id_t arg, loc_t loc) {
    return mkNode<AST_JumpStmt>(std::move(loc), jtype, arg);
}

TreeNodeRef AST_JumpStmt::getTreeNode(ParsingContext const &pctx) const {
    std::string str;
    if (type == AST_JumpStmt::J_GOTO)
        str = "goto "s + pctx.getIdentById(std::get<string_id_t>(arg));
    else if (type == AST_JumpStmt::J_BREAK)
        str = "break"s;
    else if (type == AST_JumpStmt::J_CONTINUE)
        str = "continue"s;
    else if (type == AST_JumpStmt::J_RET)
        str = "return"s;
    auto node = TreeNode::create(str);

    if (type == AST_JumpStmt::J_RET) {
        if (std::get<AST_Expr *>(arg))
            node->addChild(std::get<AST_Expr*>(arg)->getTreeNode(pctx));
    }
    else if (type == AST_JumpStmt::J_GOTO) {
        auto labelNode = TreeNode::create(std::to_string(std::get<string_id_t>(arg)));
        node->addChild(labelNode);
    }

    return node;
}

AST_Expr* AST_JumpStmt::getExpr() const {
    return std::get<AST_Expr*>(arg);
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

AST_FunctionDef* AbstractSyntaxTree::mkFunDef(AST_DeclSpecifiers *spec, AST_Declarator *decl, AST_CompoundStmt *body,
                                              loc_t loc) {
    return mkNode<AST_FunctionDef>(std::move(loc), spec, decl, body);
}

TreeNodeRef AST_FunctionDef::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("function_def"s);
    node->addChild(specifiers->getTreeNode(pctx));
    node->addChild(decl->getTreeNode(pctx));
    node->addChild(body->getTreeNode(pctx));
    return node;
}


// AST_TranslationUnit

AST_TranslationUnit::AST_TranslationUnit() : AST_Node(AST_TRANS_UNIT), children() {}

AST_TranslationUnit* AbstractSyntaxTree::mkTransUnit(loc_t loc) {
    if (top != nullptr)
        throw cw39_internal_error("Top-level AST entry has been already declared");
    top = mkNode<AST_TranslationUnit>(std::move(loc));
    return top;
}

AST_TranslationUnit* AST_TranslationUnit::append(AST_Node *node) {
    children.emplace_back(node);
    return this;
}

TreeNodeRef AST_TranslationUnit::getTreeNode(ParsingContext const &pctx) const {
    auto node = TreeNode::create("t_unit"s);
    for (auto &e : children)
        node->addChild(e->getTreeNode(pctx));
    return node;
}
