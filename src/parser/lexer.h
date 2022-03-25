#ifndef LEXER_H_INCLUDED__
#define LEXER_H_INCLUDED__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

struct YYLTYPE;

#define YY_NO_UNPUT

struct LinesWarpMap;

typedef struct lex_extra {
    int cur_line;
    int cur_column;

    struct CoreParserState *state;
    const struct LinesWarpMap *warps;
} lex_extra_t;

#define YY_USER_ACTION                              \
{                                                   \
    int i;                                          \
    struct lex_extra *extra = yyextra;              \
    yylloc->first_line = extra->cur_line;           \
    yylloc->first_column = extra->cur_column;       \
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

struct AST_TranslationUnit;
struct LinesWarpMap;

void yyerror(void*, yyscan_t, struct AST_TranslationUnit**, const struct LinesWarpMap *warps, const char *str);

void init_scanner(const char *program, yyscan_t *scanner, lex_extra_t *extra);
void destroy_scanner(yyscan_t scanner);

void lexer_error(struct YYLTYPE *loc, lex_extra_t *extra, const char *msg);


AST_Literal_t get_integer(const char *str);
AST_Literal_t get_float(const char *str);
AST_Literal_t get_charval(const char *str);


// Parser internals

#define SL(node, loc) do { (node)->setLoc(loc.first_line, loc.first_column); } while (0)

#ifdef __cplusplus
}
#endif

#endif /* LEXER_H_INCLUDED__ */
