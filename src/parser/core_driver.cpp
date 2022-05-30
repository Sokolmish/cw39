#include "core_driver.hpp"
#include <fmt/core.h>
#include <cctype>

static bool isStringWhitespace(const std::string &str) {
    return std::all_of(str.begin(), str.end(), isspace);
}

CoreDriver::CoreDriver(ParsingContext &ctx, std::string program, int flags)
        : ctx(ctx), ast(std::make_shared<AbstractSyntaxTree>()), text(std::move(program)) {
    trace_parsing = (flags & TRACE_PARSER) != 0;
    trace_scanning = (flags & TRACE_SCANNER) != 0;

    if (text.empty() || isStringWhitespace(text))
        throw cw39_error("Cannot compile empty or whitespace file");

    text += std::string("\0\0", 2); // Needed for lexer

    location.initialize(nullptr);
    scan_begin();
    yy::parser parse(scanner, *this);
    parse.set_debug_level(trace_parsing);
    int rc = parse();
    scan_end();

    if (rc != 0)
        throw parser_exception(fmt::format("Parsing failed with return code {}", rc), "");
}

std::shared_ptr<AbstractSyntaxTree> CoreDriver::getAST() {
    return ast;
}

string_id_t CoreDriver::getDeclaredIdentDirect(AST_DirDeclarator const &decl) {
    if (decl.type == AST_DirDeclarator::NAME) {
        return decl.getIdent();
    }
    else if (decl.type == AST_DirDeclarator::NESTED) {
        return getDeclaredIdent(decl.getBaseDecl());
    }
    else if (decl.type == AST_DirDeclarator::ARRAY || decl.type == AST_DirDeclarator::FUNC) {
        return getDeclaredIdentDirect(decl.getBaseDirectDecl());
    }
    else {
        throw std::logic_error("Wrong direct declarator type");
    }
}

string_id_t CoreDriver::getDeclaredIdent(AST_Declarator const &decl) {
    return getDeclaredIdentDirect(*decl.direct);
}

AST_Declaration* CoreDriver::parseDeclaration(AST_DeclSpecifiers *spec, AST_InitDeclaratorList *child,
                                              yy::location loc) {
    if (spec->storage_specifier == AST_DeclSpecifiers::ST_TYPEDEF && child) {
        auto specQualsLst = ast->mkSpecQualLst(spec->type_specifiers, spec->type_qualifiers, loc);
        for (auto const &decl : child->v) {
            string_id_t ident = getDeclaredIdent(*decl->declarator);
            auto typeName = ast->mkTypeName(specQualsLst, decl->declarator, loc);
            typesAliases.emplace(ident, ast->mkTypeSpec(typeName, loc));
        }
    }
    return ast->mkDeclaration(spec, child, std::move(loc));
}

AST_TypeSpecifier* CoreDriver::getDefinedType(string_id_t id) {
    return typesAliases.at(id);
}

bool CoreDriver::isDefinedType(string_id_t id) {
    return typesAliases.contains(id);
}


AST_Attribute CoreDriver::get_attribute(string_id_t id) const {
    std::string name = ctx.getIdentById(id);
    if (name == "noinline")
        return AST_Attribute(AST_Attribute::ATTR_NOINLINE); // TODO: it is lost anyway
    return AST_Attribute(); // Unknown attribute
}


void CoreDriver::lexer_error(std::string msg) const {
    auto fixLoc = ctx.warps.getLoc(location.begin.line);
    std::string filename = ctx.warps.getFilename(fixLoc.filenum);
    std::string excLoc = fmt::format("{}:{}:{}", filename, fixLoc.line, location.begin.column);
    std::string excMsg = fmt::format("Lexer error: {}", std::move(msg));
    throw parser_exception(std::move(excMsg), std::move(excLoc));
}


void yy::parser::error(const location_type &loc, const std::string &msg) {
    auto fixLoc = drv.ctx.warps.getLoc(loc.begin.line);
    std::string filename = drv.ctx.warps.getFilename(fixLoc.filenum);
    std::string excLoc = fmt::format("{}:{}:{}", filename, fixLoc.line, loc.begin.column);
    throw parser_exception(msg, std::move(excLoc));
}
