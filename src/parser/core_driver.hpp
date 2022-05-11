#ifndef CORE_DRIVER_HPP_INCLUDED__
#define CORE_DRIVER_HPP_INCLUDED__

#include <string>
#include <map>
#include <memory>
#include "yy_parser.hpp"
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

private:
    ParsingContext &ctx;
    std::shared_ptr<AbstractSyntaxTree> ast;
    std::string text;
    yy::location location; // TODO: fix tracking

    bool trace_parsing;
    bool trace_scanning;

    yyscan_t scanner;
    void *bufState;

    void scan_begin();
    void scan_end();
    void parse();

    [[noreturn]] void lexer_error(std::string msg);

    friend class yy::parser;
    friend YY_DECL;
};

#endif /* CORE_DRIVER_HPP_INCLUDED__ */
