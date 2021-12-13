#ifndef __PARSER_COMMON_H__
#define __PARSER_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

typedef struct lex_extra {
    int continued;
    int cur_line;
    int cur_column;
} lex_extra_t;

#define YY_NO_UNPUT

#define YY_USER_ACTION                              \
{                                                   \
    int i;                                          \
    struct lex_extra *extra = yyextra;              \
    if (! extra->continued) {                       \
        yylloc->first_line = extra->cur_line;       \
        yylloc->first_column = extra->cur_column;   \
    }                                               \
    extra->continued = 0;                           \
    for (i = 0; i < yyleng; i++) {                  \
        if (yytext[i] == '\n') {                    \
            extra->cur_line++;                      \
            extra->cur_column = 1;                  \
        }                                           \
        else {                                      \
            extra->cur_column++;                    \
        }                                           \
    }                                               \
    yylloc->last_line = extra->cur_line;            \
    yylloc->last_column = extra->cur_column;        \
}


void init_scanner(const char *program, yyscan_t *scanner, lex_extra_t *extra);
void destroy_scanner(yyscan_t scanner);

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

string_id_t get_ident_id(const char *ident, int *type);
string_id_t get_string_id(const char *str);

AST_Literal_t get_integer(const char *str);
AST_Literal_t get_float(const char *str);
AST_Literal_t get_charval(const char *str);

#ifdef __cplusplus
}
#endif

#endif /* __PARSER_COMMON_H__ */
