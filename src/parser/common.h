#ifndef __PARSER_COMMON_H__
#define __PARSER_COMMON_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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

typedef int string_id_t;
#define NO_IDENT_ID 0

struct CoreParserState;

string_id_t get_string_id(struct CoreParserState *rpstate, const char *str);
string_id_t get_ident_id(struct CoreParserState *rpstate, const char *ident, int *type);

#ifdef __cplusplus
}
#endif

#endif /* __PARSER_COMMON_H__ */
