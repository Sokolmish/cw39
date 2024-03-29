#include "generator.hpp"

std::shared_ptr<IR_Type> IR_Generator::getStructType(AST_UStructSpec const &spec) {
    // TODO: forwarding struct declaration
    // Lookup for existing struct type
    auto itFound = iunit->structs.find(spec.name);
    if (!spec.body) {
        if (itFound == iunit->structs.end())
            semanticError(spec.loc, "Unknown struct name");
        return itFound->second;
    }
    else if (itFound != iunit->structs.end()) {
        // Because of problems with typedef (or not only with it?)
        // different structures with same name not checked at all
        return itFound->second;
        // semanticError(spec.loc, "Struct with given name has been already defined");
    }

    // Create new struct type
    std::vector<IR_TypeStruct::StructField> fields;
    int curIndex = 0;
    for (auto const &structDecl : spec.body->children) {
        for (auto const &singleDecl : structDecl->child->children) {
            if (singleDecl->bitwidth)
                throw cw39_not_implemented("Bitwidth");
            auto fieldType = getType(*structDecl->type, *singleDecl->declarator);
            string_id_t fieldName = CoreDriver::getDeclaredIdent(*singleDecl->declarator);
            fields.emplace_back(fieldName, fieldType, curIndex);
            curIndex++;
        }
    }
    auto resType = std::make_shared<IR_TypeStruct>(spec.name, std::move(fields));
    iunit->structs.emplace(spec.name, resType);
    return resType;
}

std::shared_ptr<IR_Type> IR_Generator::getPrimaryType(TypeSpecifiers const &spec) {
    using ast_ts = AST_TypeSpecifier;

    if (spec.empty()) {
        throw cw39_internal_error("Empty AST_DeclSpecifiers");
    }

    if (spec[0]->spec_type == ast_ts::T_UNISTRUCT) {
        if (spec.size() != 1)
            semanticError(spec[0]->loc, "Struct specifier must be the only specifier");
        auto const &structSpec = dynamic_cast<AST_UStructSpec const &>(*spec[0]->v);
        if (structSpec.is_union)
            throw cw39_not_implemented("Unions");
        return getStructType(structSpec);
    }
    else if (spec[0]->spec_type == ast_ts::T_ENUM) {
        if (spec.size() != 1)
            semanticError(spec[0]->loc, "Enum specifier must be the only specifier");
        throw cw39_not_implemented("Enums");
    }
    else if (spec[0]->spec_type == ast_ts::T_NAMED) {
        if (spec.size() != 1)
            throw cw39_not_implemented("User defined types cannot be combined with others");
        return getType(dynamic_cast<AST_TypeName const&>(*spec[0]->v));
    }

    if (spec[0]->spec_type == ast_ts::T_VOID) {
        if (spec.size() != 1)
            semanticError(spec[0]->loc, "Void type specifier cannot be combined with others");
        return std::make_shared<IR_TypeDirect>(IR_TypeDirect::VOID);
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
                    semanticError(type->loc, "Only one type specifier is allowed");
                typeSpec = type->spec_type;
                break;

            case ast_ts::T_UNSIGNED:
            case ast_ts::T_SIGNED:
                if (sign != ast_ts::T_VOID)
                    semanticError(type->loc, "Only one signedness specifier is allowed");
                sign = type->spec_type;
                break;

            case ast_ts::T_SHORT:
                semanticError(type->loc, "Short specifier is not supported");

            case ast_ts::T_LONG:
                if (longCnt >= 2)
                    semanticError(type->loc, "More than 2 long specifiers are not allowed");
                longCnt++;
                break;

            default:
                semanticError(type->loc, "Struct, union and enum keywords cannot be combined with others");
        }
    }

    // TODO: refactor this function

    auto resType = IR_TypeDirect::VOID;
    if (isInList(typeSpec, ast_ts::T_INT, ast_ts::T_VOID)) { // Void here because it is default value
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
            semanticError(spec[0]->loc, "Char types cannot have size modificators");
        resType = sign == ast_ts::T_UNSIGNED ? IR_TypeDirect::U8 : IR_TypeDirect::I8;
    }
    else if (isInList(typeSpec, ast_ts::T_FLOAT, ast_ts::T_DOUBLE)) {
        if (sign != ast_ts::T_VOID)
            semanticError(spec[0]->loc, "Float point types cannot have signedness");
        if (longCnt != 0)
            semanticError(spec[0]->loc, "Float point types cannot have size modificators");
        if (typeSpec == ast_ts::T_FLOAT)
            resType = IR_TypeDirect::F32;
        else
            resType = IR_TypeDirect::F64;
    }
    else {
        throw std::logic_error("Wrong type");
    }

    return std::make_shared<IR_TypeDirect>(resType);
}

template <typename DeclaratorType>
std::shared_ptr<IR_Type> IR_Generator::getIndirectType(DeclaratorType const *decl,
                                                       std::shared_ptr<IR_Type> base) {

    if (!decl) {
        return base;
    }

    std::shared_ptr<IR_Type> lastPtr = std::move(base);
    for (auto curAstPtr = decl->ptr; curAstPtr; curAstPtr = curAstPtr->child) {
        auto newPtr = std::make_shared<IR_TypePtr>(std::move(lastPtr));
        if (decl->ptr->qualifiers) {
            newPtr->is_const = decl->ptr->qualifiers->is_const;
            newPtr->is_restrict = decl->ptr->qualifiers->is_restrict;
            newPtr->is_volatile = decl->ptr->qualifiers->is_volatile;
        }
        lastPtr = std::move(newPtr);
    }

    if constexpr (std::is_same<DeclaratorType, AST_Declarator>()) {
        return getDirType(*decl->direct, std::move(lastPtr));
    }
    else if constexpr (std::is_same<DeclaratorType, AST_AbstrDeclarator>()) {
        return getDirAbstrType(decl->direct, std::move(lastPtr));
    }
    else {
        static_assert(! std::is_same<DeclaratorType, DeclaratorType>(),
                      "Something went wrong with codegeneration");
        throw cw39_internal_error("Something went wrong with codegeneration");
    }
}

std::shared_ptr<IR_Type> IR_Generator::getDirType(AST_DirDeclarator const &decl,
                                                  std::shared_ptr<IR_Type> base) {
    if (decl.type == AST_DirDeclarator::NAME) {
        return base; // Name is ignored there
    }
    else if (decl.type == AST_DirDeclarator::NESTED) {
        return getIndirectType(&decl.getBaseDecl(), std::move(base));
    }
    else if (decl.type == AST_DirDeclarator::ARRAY) {
        auto sizeExpr = evalConstantExpr(*decl.arr_size);
        if (!sizeExpr.has_value())
            semanticError(decl.arr_size->loc, "Non constant array size");

        auto dirSizeExpr = sizeExpr->getType()->castType<IR_TypeDirect>();
        if (!dirSizeExpr)
            semanticError(decl.arr_size->loc, "Non constant array size (not direct value)");
        if (!dirSizeExpr->isInteger())
            semanticError(decl.arr_size->loc, "Non integer array size");

        auto size = sizeExpr->castValTo<uint64_t>();
        auto arr = std::make_shared<IR_TypeArray>(std::move(base), size);
        return getDirType(decl.getBaseDirectDecl(), arr);
    }
    else if (decl.type == AST_DirDeclarator::FUNC) {
        if (decl.func_args && !checkVoidParam(decl.func_args->v->v)) {
            std::vector<std::shared_ptr<IR_Type>> params;
            for (const auto &param: decl.func_args->v->v)
                params.push_back(getType(*param->specifiers, *param->child));
            bool isVararg = decl.func_args->has_ellipsis;
            auto func = std::make_shared<IR_TypeFunc>(std::move(base), std::move(params), isVararg);
            return getDirType(decl.getBaseDirectDecl(), func);
        }
        else {
            auto func = std::make_shared<IR_TypeFunc>(std::move(base));
            return getDirType(decl.getBaseDirectDecl(), func);
        }
    }
    else {
        throw std::logic_error("Unknown direct declarator type");
    }
}

std::shared_ptr<IR_Type> IR_Generator::getDirAbstrType(AST_DirAbstrDeclarator const *decl,
                                                       std::shared_ptr<IR_Type> base) {
    if (!decl) {
        return base;
    }
    else if (decl->type == AST_DirAbstrDeclarator::NESTED) {
        return getIndirectType(&decl->getBaseDecl(), std::move(base));
    }
    else if (decl->type == AST_DirAbstrDeclarator::ARRAY) {
        auto sizeExpr = evalConstantExpr(*decl->arr_size);
        if (!sizeExpr.has_value())
            semanticError(decl->arr_size->loc, "Non constant array size");

        auto dirSizeExpr = sizeExpr->getType()->castType<IR_TypeDirect>();
        if (!dirSizeExpr)
            semanticError(decl->arr_size->loc, "Non constant array size (not direct value)");
        if (!dirSizeExpr->isInteger())
            semanticError(decl->arr_size->loc, "Non integer array size");

        auto size = sizeExpr->castValTo<uint64_t>();
        auto arr = std::make_shared<IR_TypeArray>(std::move(base), size);
        return getDirAbstrType(&decl->getBaseDirectDecl(), arr);
    }
    else if (decl->type == AST_DirAbstrDeclarator::FUNC) {
        if (decl->func_args && !checkVoidParam(decl->func_args->v->v)) {
            std::vector<std::shared_ptr<IR_Type>> params;
            for (const auto &param: decl->func_args->v->v)
                params.push_back(getType(*param->specifiers, *param->child));
            bool isVararg = decl->func_args->has_ellipsis;
            auto func = std::make_shared<IR_TypeFunc>(std::move(base), std::move(params), isVararg);
            return getDirAbstrType(&decl->getBaseDirectDecl(), func);
        }
        else {
            auto func = std::make_shared<IR_TypeFunc>(std::move(base));
            return getDirAbstrType(&decl->getBaseDirectDecl(), func);
        }
    }
    throw std::logic_error("Unknown direct abstract declarator type");
}

std::shared_ptr<IR_Type> IR_Generator::getType(AST_DeclSpecifiers const &spec, AST_Declarator const &decl) {
    return getIndirectType(&decl, getPrimaryType(spec.type_specifiers));
}

std::shared_ptr<IR_Type> IR_Generator::getType(AST_SpecsQualsList const &spec, AST_Declarator const &decl) {
    return getIndirectType(&decl, getPrimaryType(spec.type_specifiers));
}

std::shared_ptr<IR_Type> IR_Generator::getType(AST_TypeName const &typeName) {
    auto primType = getPrimaryType(typeName.qual->type_specifiers);
    if (!typeName.declarator)
        return primType;
    return std::visit([&, this](auto const &decl) {
        return getIndirectType(decl, std::move(primType));
    }, *typeName.declarator);
}

/** Check for explicitly no parameters via `void`, as in `int f(void)` */
bool IR_Generator::checkVoidParam(std::vector<AST_ParameterDeclaration*> const &params) {
    for (auto const &arg : params) {
        if (arg->child) // Has declarator
            continue;
        auto const &typeSpecs = arg->specifiers->type_specifiers;
        if (typeSpecs.size() != 1)
            continue;
        if (typeSpecs[0]->spec_type == AST_TypeSpecifier::T_VOID) {
            if (params.size() != 1)
                semanticError(arg->loc, "Void must be the only parameter");
            return true;
        }
    }
    return false;
}

std::vector<IR_Generator::IR_FuncArgument> IR_Generator::getDeclaredFuncArgs(AST_Declarator const &decl) {
    if (decl.direct->type != AST_DirDeclarator::FUNC)
        semanticError(decl.direct->loc, "Non function type");
    if (!decl.direct->func_args)
        return std::vector<IR_FuncArgument>();

    if (checkVoidParam(decl.direct->func_args->v->v))
        return std::vector<IR_FuncArgument>();

    std::vector<IR_FuncArgument> res;
    for (auto const &arg : decl.direct->func_args->v->v) {
        auto ident = CoreDriver::getDeclaredIdent(*arg->child);
        auto type = getType(*arg->specifiers, *arg->child);
        res.push_back({ ident, type });
    }
    return res;
}

std::shared_ptr<IR_Type> IR_Generator::getLiteralType(AST_Literal const &lit) {
    if (lit.type == AST_Literal::INTEGER) {
        if (lit.isUnsigned) {
            if (lit.longCnt)
                return std::make_shared<IR_TypeDirect>(IR_TypeDirect::U64);
            else
                return std::make_shared<IR_TypeDirect>(IR_TypeDirect::U32);
        }
        else { // Signed
            if (lit.longCnt)
                return std::make_shared<IR_TypeDirect>(IR_TypeDirect::I64);
            else
                return std::make_shared<IR_TypeDirect>(IR_TypeDirect::I32);
        }
    }
    else if (lit.type == AST_Literal::FLOAT) {
        if (lit.isFloat)
            return std::make_shared<IR_TypeDirect>(IR_TypeDirect::F32);
        else {
            return std::make_shared<IR_TypeDirect>(IR_TypeDirect::F64);
        }
    }
    else if (lit.type == AST_Literal::CHARACTER) {
        return std::make_shared<IR_TypeDirect>(IR_TypeDirect::I8);
    }
    else {
        throw std::logic_error("Wrong literal type");
    }
}

IntermediateUnit::FunLinkage IR_Generator::getFunLinkage(AST_DeclSpecifiers::StorageSpec spec,
                                                         yy::location const &loc) {
    switch (spec) {
        case AST_DeclSpecifiers::ST_NONE:
        case AST_DeclSpecifiers::ST_EXTERN:
            return IntermediateUnit::FunLinkage::EXTERN;
        case AST_DeclSpecifiers::ST_STATIC:
            return IntermediateUnit::FunLinkage::STATIC;
        case AST_DeclSpecifiers::ST_WEAK:
            return IntermediateUnit::FunLinkage::WEAK;
        case AST_DeclSpecifiers::ST_AUTO:
            semanticError(loc, "Function declaration cannot has 'auto' storage specifier");
        case AST_DeclSpecifiers::ST_REGISTER:
            semanticError(loc, "Function declaration cannot has 'register' storage specifier");
        case AST_DeclSpecifiers::ST_TYPEDEF:
            throw cw39_internal_error("Typedef in wrong context");
    }
    throw std::logic_error("Unknown storage specifier");
}

IntermediateUnit::VarLinkage IR_Generator::getGVarLinkage(AST_DeclSpecifiers::StorageSpec spec,
                                                         yy::location const &loc) {
    switch (spec) {
        case AST_DeclSpecifiers::ST_NONE:
            return IntermediateUnit::VarLinkage::DEFAULT;
        case AST_DeclSpecifiers::ST_EXTERN:
            return IntermediateUnit::VarLinkage::EXTERN;
        case AST_DeclSpecifiers::ST_STATIC:
            return IntermediateUnit::VarLinkage::STATIC;
        case AST_DeclSpecifiers::ST_WEAK:
            return IntermediateUnit::VarLinkage::WEAK;
        case AST_DeclSpecifiers::ST_AUTO:
            semanticError(loc, "Global declaration cannot has 'auto' storage specifier");
        case AST_DeclSpecifiers::ST_REGISTER:
            semanticError(loc, "Global declaration cannot has 'register' storage specifier");
        case AST_DeclSpecifiers::ST_TYPEDEF:
            throw cw39_internal_error("Typedef in wrong context");
    }
    throw std::logic_error("Unknown storage specifier");
}
