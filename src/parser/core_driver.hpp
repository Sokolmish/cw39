#ifndef CORE_DRIVER_HPP_INCLUDED__
#define CORE_DRIVER_HPP_INCLUDED__

#include <string>
#include <map>
#include <memory>
#include "yy_parser.hpp"
#include "common.hpp"
#include "parsing_context.hpp"
#include "utils.hpp"

using yyscan_t = void*;

#define YY_DECL yy::parser::symbol_type yycore_lex(yyscan_t yyscanner, CoreDriver& drv)
YY_DECL;

class parser_exception : public cw39_exception {
public:
    parser_exception(std::string msg, std::string loc) // TODO: location ewerywhere
            : cw39_exception("Parser error", std::move(loc), std::move(msg)) {}
};

class CoreDriver {
public:
    enum ParserDebugFlags {
        TRACE_SCANNER = 0x1,
        TRACE_PARSER  = 0x2,
    };

    CoreDriver(ParsingContext &ctx, std::string program, int debugFlags);

    std::shared_ptr<AbstractSyntaxTree> getAST();
    std::shared_ptr<AbstractSyntaxTree> moveAST() &&;

    static string_id_t getDeclaredIdentDirect(AST_DirDeclarator const &decl);
    static string_id_t getDeclaredIdent(AST_Declarator const &decl);

private:
    ParsingContext &ctx;
    std::shared_ptr<AbstractSyntaxTree> ast;
    std::string text; // This string will be changed by lexer and cannot be used after it
    yy::location location;

    std::map<string_id_t, AST_TypeSpecifier*> typesAliases;

    bool trace_parsing;
    bool trace_scanning;

    yyscan_t scanner;
    void *bufState;

    void scan_begin();
    void scan_end();

    /** Check for typedef. Pass it to AST anyway (it will be dropped in generator). */
    AST_Declaration* parseDeclaration(AST_DeclSpecifiers *spec, AST_InitDeclaratorList *child, yy::location loc);
    bool isDefinedType(string_id_t id);
    AST_TypeSpecifier* getDefinedType(string_id_t id);

    AST_Attribute get_attribute(string_id_t id) const;

    AST_Literal get_integer(const char *str) const;
    AST_Literal get_float(const char *str, size_t len) const;
    AST_Literal get_charval(const char *str, size_t len) const;

    [[noreturn]] void lexer_error(std::string msg) const;

    friend class yy::parser;
    friend YY_DECL;
};

#endif /* CORE_DRIVER_HPP_INCLUDED__ */
