#ifndef CORE_DRIVER_HPP_INCLUDED__
#define CORE_DRIVER_HPP_INCLUDED__

#include <string>
#include <map>
#include <memory>
#include "warps.hpp"

#include "yy_parser.hpp"
#include "common.hpp"

using yyscan_t = void*;

#define YY_DECL yy::parser::symbol_type yycore_lex(yyscan_t yyscanner, CoreDriver& drv)

YY_DECL;


struct CoreParserState {
    struct IdentInfo {
        string_id_t id;
        enum : int { IDENT, DEF_TYPE } type = IDENT;
        explicit IdentInfo(string_id_t id) : id(id) {}
    };

    struct StringInfo {
        string_id_t id;
        explicit StringInfo(string_id_t id) : id(id) {}
    };

    std::map<std::string, IdentInfo, std::less<>> identsMap;
    std::map<std::string, StringInfo, std::less<>> stringsMap;

    std::map<string_id_t, std::string> invIdentsMap;
    std::map<string_id_t, std::string> invStringsMap;

    string_id_t idCnt = 1;
    string_id_t strCnt = 1;
};


class CoreDriver {
public:
    CoreDriver(std::string program, LinesWarpMap const &linesWarps);

    std::shared_ptr<AST_TranslationUnit> getTransUnit();
    CoreParserState* getPState();

private:
    void scan_begin();
    void scan_end();
    void lexer_error(const char *msg);

    void parse();

    bool trace_parsing;
    bool trace_scanning;

    std::string text;
    LinesWarpMap const &warps;

    yy::location location;

    std::unique_ptr<CoreParserState> pstate;
    AST_TranslationUnit *res;
    std::shared_ptr<AST_TranslationUnit> transUnit;

    yyscan_t scanner;
    void *bufState;

    friend class yy::parser;
    friend YY_DECL;
};

#endif /* CORE_DRIVER_HPP_INCLUDED__ */
