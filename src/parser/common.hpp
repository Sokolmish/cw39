#ifndef PARSER_COMMON_HPP_INCLUDED__
#define PARSER_COMMON_HPP_INCLUDED__

#include <string>
#include <cstdint>

enum AST_LiteralType {
    INTEGER_LITERAL, FLOAT_LITERAL, CHAR_LITERAL
};

typedef struct AST_Literal {
    enum AST_LiteralType type;
    int longCnt;
    int isUnsigned;
    int isFloat;
    union {
        char v_char;
        uint32_t vu32;
        uint64_t vu64;
        int32_t vi32;
        int64_t vi64;
        float vf32;
        double vf64;
    } val;
} AST_Literal_t;

using string_id_t = int;
#define NO_IDENT_ID 0

struct CoreParserState;

enum class IdentType { IDENT, TYPENAME };

string_id_t get_string_id(struct CoreParserState *rpstate, const char *str);
string_id_t get_ident_id(struct CoreParserState *rpstate, const char *ident, IdentType *type);

std::string get_ident_by_id(CoreParserState *rpstate, string_id_t id);
std::string get_string_by_id(CoreParserState *rpstate, string_id_t id);

#endif /* PARSER_COMMON_HPP_INCLUDED__ */
