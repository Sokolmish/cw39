#ifndef PARSER_COMMON_HPP_INCLUDED__
#define PARSER_COMMON_HPP_INCLUDED__

#include <string>
#include <cstdint>

struct AST_Literal {
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
};

struct AST_Attribute {
    enum AttrTypes {
        ATTR_NONE,
        ATTR_NOINLINE,
    } attr_type;

    AST_Attribute() : attr_type(ATTR_NONE) {}
    AST_Attribute(AttrTypes type) : attr_type(type) {}
};

using string_id_t = int;
#define NO_IDENT_ID 0

/** Doesn't unescaping '\xhh' */
inline char unescapeChar(char ch) {
    switch (ch) {
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 'f':
            return '\f';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 't':
            return '\t';
        case 'v':
            return '\v';
        case '\\':
            return '\\';
        case '\'':
            return '\'';
        case '\"':
            return '\"';
        case '?':
            return '\?';
        default:
            return ch;
    }
}

/** Parses '\xHH' sequence. Assumes that c1 and c2 are xdigits. */
inline char parseXEscape(char c1, char c2) {
//    std::array buf{ c1, c2, '\0' };
//    return (char)strtol(buf.data(), nullptr, 16);
#define get_xdigit(c) c - (c < 'A' ? '0' : (c < 'a' ? 'A' : 'a') - 10)
    char v1 = get_xdigit(c1);
    char v2 = get_xdigit(c2);
#undef get_xdigit
    return (v1 << 4) | v2;
}

#endif /* PARSER_COMMON_HPP_INCLUDED__ */
