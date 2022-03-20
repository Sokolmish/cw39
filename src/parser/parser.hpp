#ifndef PARSER_HPP_INCLUDED__
#define PARSER_HPP_INCLUDED__

#include "ast.hpp"
#include "preprocessor.hpp"
#include "common.hpp"
#include <map>
#include <string>

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

class CoreParser {
public:
    CoreParser(std::string const &str, LinesWarpMap &warps);

    std::shared_ptr<AST_TranslationUnit> getTransUnit();
    CoreParserState* getPState();

private:
    std::shared_ptr<AST_TranslationUnit> unit;
    std::unique_ptr<CoreParserState> pstate;

    AST_TranslationUnit* parse_program(std::string const &str, CoreParserState *state, const LinesWarpMap *warps);
};

#endif /* PARSER_HPP_INCLUDED__ */
