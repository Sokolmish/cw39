#ifndef CORE_DRIVER_HPP_INCLUDED__
#define CORE_DRIVER_HPP_INCLUDED__

#include <string>
#include <map>
#include <memory>

#include "yy_parser.hpp"
#include "parsing_context.hpp"

using yyscan_t = void*;

#define YY_DECL yy::parser::symbol_type yycore_lex(yyscan_t yyscanner, CoreDriver& drv)
YY_DECL;

class CoreDriver {
public:
    CoreDriver(ParsingContext &ctx, std::string program);

    std::shared_ptr<AST_TranslationUnit> getTransUnit();

private:
    void scan_begin();
    void scan_end();
    void lexer_error(const char *msg);

    void parse();

    bool trace_parsing;
    bool trace_scanning;

    ParsingContext &ctx;

    std::string text;

    yy::location location; // TODO: fix tracking

    AST_TranslationUnit *res;
    std::shared_ptr<AST_TranslationUnit> transUnit;

    yyscan_t scanner;
    void *bufState;

    friend class yy::parser;
    friend YY_DECL;
};

#endif /* CORE_DRIVER_HPP_INCLUDED__ */
