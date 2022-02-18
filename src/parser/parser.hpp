#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include "ast.hpp"
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
    explicit CoreParser(std::string const &str);

    std::shared_ptr<AST_TranslationUnit> getTransUnit();
    CoreParserState* getPState();

private:
    std::shared_ptr<AST_TranslationUnit> unit;
    std::unique_ptr<CoreParserState> pstate;

    AST_TranslationUnit* parse_program(std::string const &str, CoreParserState *state);
};

#endif /* __PARSER_HPP__ */
