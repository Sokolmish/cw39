#include "generator.hpp"

static std::unique_ptr<IR_Type> getPrimaryType(std::vector<std::unique_ptr<AST_TypeSpecifier>> const &spec) {
    using ast_ts = AST_TypeSpecifier;

    if (spec.empty()) {
        semanticError("Empty AST_DeclSpecifiers");
    }

    if (spec[0]->spec_type == ast_ts::T_UNISTRUCT) {
        NOT_IMPLEMENTED("");
    }
    else if (spec[0]->spec_type == ast_ts::T_ENUM) {
        NOT_IMPLEMENTED("");
    }
    else if (spec[0]->spec_type == ast_ts::T_NAMED) {
        NOT_IMPLEMENTED("");
    }

    if (spec[0]->spec_type == ast_ts::T_VOID) {
        if (spec.size() != 1)
            semanticError("Void type specifier cannot be combined with others");
        return std::make_unique<IR_TypeDirect>(IR_TypeDirect::VOID);
    }

    int longCnt = 0;
    ast_ts::TypeSpec sign = ast_ts::T_VOID; // T_VOID is guaranteed not to be assigned to this
    ast_ts::TypeSpec typeSpec = ast_ts::T_VOID;

    for (auto const &type : spec) {
        switch (type->spec_type) {
            case ast_ts::T_CHAR:
            case ast_ts::T_INT:
            case ast_ts::T_FLOAT:
            case ast_ts::T_DOUBLE:
                if (typeSpec != ast_ts::T_VOID)
                    semanticError("Only one type specifier is allowed");
                typeSpec = type->spec_type;
                break;

            case ast_ts::T_UNSIGNED:
            case ast_ts::T_SIGNED:
                if (sign != ast_ts::T_VOID)
                    semanticError("Only one signedness specifier is allowed");
                sign = type->spec_type;
                break;

            case ast_ts::T_SHORT:
                semanticError("Short specifier is unsupported");

            case ast_ts::T_LONG:
                if (longCnt >= 2)
                    semanticError("More than 2 long specifiers are not allowed");
                longCnt++;
                break;

            default:
                semanticError("Struct, union and enum keywords cannot be combined with others");
        }
    }

    // TODO: refactor this function

    auto resType = IR_TypeDirect::VOID;
    if (isInList(typeSpec, { ast_ts::T_INT, ast_ts::T_VOID })) { // Void here because it is default value
        if (sign == ast_ts::T_UNSIGNED) {
            if (longCnt != 0)
                resType = IR_TypeDirect::U64;
            else
                resType = IR_TypeDirect::U32;
        }
        else {
           if (longCnt != 0)
                resType = IR_TypeDirect::I64;
           else
                resType = IR_TypeDirect::I32;
        }
    }
    else if (typeSpec == ast_ts::T_CHAR) {
        if (longCnt != 0)
            semanticError("Char types cannot have size modificators");
        resType = sign == ast_ts::T_UNSIGNED ? IR_TypeDirect::U8 : IR_TypeDirect::I8;
    }
    else if (isInList(typeSpec, { ast_ts::T_FLOAT, ast_ts::T_DOUBLE })) {
        if (sign != ast_ts::T_VOID)
            semanticError("Float point types cannot have signedness");
        if (longCnt != 0)
            semanticError("Float point types cannot have size modificators");
        resType = IR_TypeDirect::F32;
    }
    else {
        semanticError("WTF type");
    }

    return std::make_unique<IR_TypeDirect>(resType);
}

static std::unique_ptr<IR_Type> getDirectType(
        AST_DirectDeclarator const &decl, std::unique_ptr<IR_Type> base);
static std::unique_ptr<IR_Type> getDirectAbstractType(
        AST_DirectAbstractDeclarator const &decl, std::unique_ptr<IR_Type> base);

template <typename DeclaratorType>
static std::unique_ptr<IR_Type> getIndirectType(
        DeclaratorType const *decl, std::unique_ptr<IR_Type> base) {

    if (!decl) {
        return base;
    }

    std::unique_ptr<IR_Type> lastPtr = std::move(base);
    for (auto curAstPtr = decl->ptr.get(); curAstPtr; curAstPtr = curAstPtr->child.get()) {
        auto newPtr = std::make_unique<IR_TypePtr>(std::move(lastPtr));
        if (decl->ptr->qualifiers) {
            newPtr->is_const = decl->ptr->qualifiers->is_const;
            newPtr->is_restrict = decl->ptr->qualifiers->is_restrict;
            newPtr->is_volatile = decl->ptr->qualifiers->is_volatile;
        }
        lastPtr = std::move(newPtr);
    }

    if constexpr (std::is_same<DeclaratorType, AST_Declarator>()) {
        return getDirectType(*decl->direct.get(), std::move(lastPtr));
    }
    else if constexpr (std::is_same<DeclaratorType, AST_AbstractDeclarator>()) {
        return getDirectAbstractType(*decl->direct.get(), std::move(lastPtr));
    }
    else {
        static_assert(! std::is_same<DeclaratorType, DeclaratorType>(),
                      "Something went wrong with codegeneration");
        throw; // Just in case
    }
}

static std::unique_ptr<IR_Type> getDirectType(AST_DirectDeclarator const &decl, std::unique_ptr<IR_Type> base) {
    if (decl.type == AST_DirectDeclarator::NAME) {
        return base; // Name is ignored there
    }
    else if (decl.type == AST_DirectDeclarator::NESTED) {
        return getIndirectType(
                dynamic_cast<AST_Declarator const *>(
                        std::get<std::unique_ptr<AST_Node>>(decl.base).get()),
                std::move(base));
    }
    else if (decl.type == AST_DirectDeclarator::ARRAY) {
        auto sizeExpr = evalConstantExpr(*decl.arr_size);
        if (!sizeExpr.has_value())
            semanticError("Non constant array size");
        if (sizeExpr->getType()->type != IR_Type::DIRECT)
            semanticError("Non constant array size (not direct value)");
        if (!dynamic_cast<IR_TypeDirect const &>(*sizeExpr->getType()).isInteger())
            semanticError("Non integer array size");
        auto size = sizeExpr->castValTo<uint64_t>();
        return std::make_unique<IR_TypeArray>(std::move(base), size);
    }
    else if (decl.type == AST_DirectDeclarator::FUNC) {
        if (decl.func_args) {
            std::vector<std::shared_ptr<IR_Type>> params;
            for (const auto &param: decl.func_args->v->v)
                params.push_back(getType(*param->specifiers, *param->child));
            bool isVararg = decl.func_args->has_ellipsis;
            return std::make_unique<IR_TypeFunc>(std::move(base), std::move(params), isVararg);
        }
        else {
            return std::make_unique<IR_TypeFunc>(std::move(base));
        }
    }
    else {
        semanticError("Unknown direct declarator type");
    }
}

static std::unique_ptr<IR_Type> getDirectAbstractType(
        AST_DirectAbstractDeclarator const &decl, std::unique_ptr<IR_Type> base) {

    if (decl.type == AST_DirectAbstractDeclarator::NESTED) {
        return getIndirectType(
                dynamic_cast<AST_AbstractDeclarator const *>(decl.base.get()),
                std::move(base));
    }
    else if (decl.type == AST_DirectAbstractDeclarator::ARRAY) {
        auto sizeExpr = evalConstantExpr(*decl.arr_size);
        if (!sizeExpr.has_value())
            semanticError("Non constant array size");
        if (sizeExpr->getType()->type != IR_Type::DIRECT)
            semanticError("Non constant array size (not direct value)");
        if (!dynamic_cast<IR_TypeDirect const &>(*sizeExpr->getType()).isInteger())
            semanticError("Non integer array size");
        auto size = sizeExpr->castValTo<uint64_t>();
        return std::make_unique<IR_TypeArray>(std::move(base), size);
    }
    else if (decl.type == AST_DirectAbstractDeclarator::FUNC) {
        if (decl.func_args) {
            std::vector<std::shared_ptr<IR_Type>> params;
            for (const auto &param: decl.func_args->v->v)
                params.push_back(getType(*param->specifiers, *param->child));
            bool isVararg = decl.func_args->has_ellipsis;
            return std::make_unique<IR_TypeFunc>(std::move(base), std::move(params), isVararg);
        }
        else {
            return std::make_unique<IR_TypeFunc>(std::move(base));
        }
    }
    semanticError("Unknown direct abstract declarator type");
}

std::shared_ptr<IR_Type> getType(AST_DeclSpecifiers const &spec, AST_Declarator const &decl) {
    return getIndirectType(&decl, getPrimaryType(spec.type_specifiers));
}

std::shared_ptr<IR_Type> getType(AST_TypeName const &typeName) {
    auto base = getPrimaryType(typeName.qual->type_specifiers);
    return getIndirectType(typeName.declarator.get(), std::move(base));
}

static string_id_t getDeclaredIdentDirect(AST_DirectDeclarator const &decl) {
    if (decl.type == AST_DirectDeclarator::NAME) {
        return std::get<string_id_t>(decl.base);
    }
    else if (decl.type == AST_DirectDeclarator::NESTED) {
        auto const &node = std::get<std::unique_ptr<AST_Node>>(decl.base);
        return getDeclaredIdent(dynamic_cast<AST_Declarator const &>(*node));
    }
    else if (decl.type == AST_DirectDeclarator::ARRAY || decl.type == AST_DirectDeclarator::FUNC) {
        auto const &node = std::get<std::unique_ptr<AST_Node>>(decl.base);
        return getDeclaredIdentDirect(dynamic_cast<AST_DirectDeclarator const &>(*node));
    }
    else {
        semanticError("Wrong direct declarator type");
    }
}

string_id_t getDeclaredIdent(AST_Declarator const &decl) {
    return getDeclaredIdentDirect(*decl.direct);
}

std::vector<IR_FuncArgument> getDeclaredFuncArguments(AST_Declarator const &decl) {
    if (decl.direct->type != AST_DirectDeclarator::FUNC)
        semanticError("Non function type");
    if (!decl.direct->func_args)
        return std::vector<IR_FuncArgument>();

    std::vector<IR_FuncArgument> res;
    for (auto const &arg : decl.direct->func_args->v->v) {
        auto ident = getDeclaredIdent(*arg->child);
        auto type = getType(*arg->specifiers, *arg->child);
        res.push_back({ ident, type });
    }
    return  res;
}

std::shared_ptr<IR_Type> getLiteralType(AST_Literal const &lit) {
    if (lit.type == INTEGER_LITERAL) {
        if (lit.isUnsigned) {
            if (lit.longCnt)
                return std::make_unique<IR_TypeDirect>(IR_TypeDirect::U64);
            else
                return std::make_unique<IR_TypeDirect>(IR_TypeDirect::U32);
        }
        else { // Signed
            if (lit.longCnt)
                return std::make_unique<IR_TypeDirect>(IR_TypeDirect::I64);
            else
                return std::make_unique<IR_TypeDirect>(IR_TypeDirect::I32);
        }
    }
    else if (lit.type == FLOAT_LITERAL) {
        if (lit.isFloat)
            return std::make_unique<IR_TypeDirect>(IR_TypeDirect::F32);
        else {
//            return std::make_unique<IR_TypeDirect>(IR_TypeDirect::F64);
            NOT_IMPLEMENTED("double");
        }
    }
    else if (lit.type == CHAR_LITERAL) {
        return std::make_unique<IR_TypeDirect>(IR_TypeDirect::I8);
    }
    else {
        semanticError("Unknown literal type");
    }
}
