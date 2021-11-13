#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include "ast.hpp"
#include <map>
#include <string>
#include <string_view>

struct IdentInfo {
    string_id_t id;
    enum : int { IDENT, DEF_TYPE } type = IDENT;
    explicit IdentInfo(string_id_t id) : id(id) {}
};

struct StringInfo {
    string_id_t id;
    explicit StringInfo(string_id_t id) : id(id) {}
};

extern std::map<std::string, IdentInfo, std::less<>> identifiers_map;
extern std::map<std::string, StringInfo, std::less<>> strings_map;
// extern std::map<std::string, IdentInfo, std::less<>> types_map;

std::string get_ident_by_id(string_id_t id);
std::string get_string_by_id(string_id_t id);

AST_TranslationUnit* parse_program(std::string const &str);

#endif /* __PARSER_HPP__ */
