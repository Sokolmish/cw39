#ifndef PARSER_COMMON_HPP_INCLUDED__
#define PARSER_COMMON_HPP_INCLUDED__

#include <string>
#include <cstdint>

typedef struct AST_Literal {
    enum LiteralType { INTEGER, FLOAT, CHARACTER };

    LiteralType type;
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

enum class IdentType { IDENT, TYPENAME };

#endif /* PARSER_COMMON_HPP_INCLUDED__ */
