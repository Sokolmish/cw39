%option reentrant
%option nodefault
%option warn
%option noyywrap nounput noinput
%option batch
%option debug
%option prefix="yycore"

%{
    #include <cstdlib>
    #include <string>

    #include "core_driver.hpp"
    #include "lexer.hpp"
    #include "yy_parser.hpp"

    #define YY_USER_ACTION loc.columns(yyleng);
%}

D           [0-9]
L           [a-zA-Z_]
H           [a-fA-F0-9]
EXP         ([Ee][\-+]?[0-9]+)
FSF         [fFlL]
ISF         (u|U|(u|U)?(ll?|LL?)|(ll?|LL?)(u|U))

%%

%{
    // A handy shortcut to the location held by the driver.
    yy::location& loc = drv.location;
    // Code run each time yylex is called.
    loc.step();
%}

"auto"                      return yy::parser::make_AUTO(loc);
"break"                     return yy::parser::make_BREAK(loc);
"case"                      return yy::parser::make_CASE(loc);
"char"                      return yy::parser::make_CHAR(loc);
"const"                     return yy::parser::make_CONST(loc);
"continue"                  return yy::parser::make_CONTINUE(loc);
"default"                   return yy::parser::make_DEFAULT(loc);
"do"                        return yy::parser::make_DO(loc);
"double"                    return yy::parser::make_DOUBLE(loc);
"else"                      return yy::parser::make_ELSE(loc);
"enum"                      return yy::parser::make_ENUM(loc);
"extern"                    return yy::parser::make_EXTERN(loc);
"float"                     return yy::parser::make_FLOAT(loc);
"for"                       return yy::parser::make_FOR(loc);
"goto"                      return yy::parser::make_GOTO(loc);
"if"                        return yy::parser::make_IF(loc);
"inline"                    return yy::parser::make_INLINE(loc);
"int"                       return yy::parser::make_INT(loc);
"long"                      return yy::parser::make_LONG(loc);
"register"                  return yy::parser::make_REGISTER(loc);
(__)?"restrict"             return yy::parser::make_RESTRICT(loc);
"return"                    return yy::parser::make_RETURN(loc);
"short"                     return yy::parser::make_SHORT(loc);
"signed"                    return yy::parser::make_SIGNED(loc);
"sizeof"                    return yy::parser::make_SIZEOF(loc);
"static"                    return yy::parser::make_STATIC(loc);
"struct"                    return yy::parser::make_STRUCT(loc);
"switch"                    return yy::parser::make_SWITCH(loc);
"typedef"                   return yy::parser::make_TYPEDEF(loc);
"union"                     return yy::parser::make_UNION(loc);
"unsigned"                  return yy::parser::make_UNSIGNED(loc);
"void"                      return yy::parser::make_VOID(loc);
"volatile"                  return yy::parser::make_VOLATILE(loc);
"while"                     return yy::parser::make_WHILE(loc);

{L}({L}|{D})*               {
                                IdentType type;
                                string_id_t id = drv.ctx.getIdentId(yytext, yyleng, &type);
                                if (type == IdentType::IDENT)
                                    return yy::parser::make_IDENTIFIER(id, loc);
                                else
                                    return yy::parser::make_TYPE_NAME(id, loc);
                            }

0[xX]{H}+{ISF}?             return yy::parser::make_CONSTANT(get_integer(yytext), loc);
0[0-7]*{ISF}?               return yy::parser::make_CONSTANT(get_integer(yytext), loc);
[1-9]{D}*{ISF}?             return yy::parser::make_CONSTANT(get_integer(yytext), loc);

{D}+{EXP}{FSF}?             return yy::parser::make_CONSTANT(get_float(yytext, yyleng), loc);
{D}*"."{D}+{EXP}?{FSF}?     return yy::parser::make_CONSTANT(get_float(yytext, yyleng), loc);
{D}+"."{D}*{EXP}?{FSF}?     return yy::parser::make_CONSTANT(get_float(yytext, yyleng), loc);

'(\\.|[^\\'\n])'            return yy::parser::make_CONSTANT(get_charval(yytext, yyleng), loc);

\"(\\.|[^\\"\n])*\"         {
                                string_id_t id = drv.ctx.getStringId(yytext, yyleng);
                                return yy::parser::make_STRING_LITERAL(id, loc);
                            }

"..."           return yy::parser::make_ELLIPSIS(loc);
">>="           return yy::parser::make_RIGHT_ASSIGN(loc);
"<<="           return yy::parser::make_LEFT_ASSIGN(loc);
"+="            return yy::parser::make_ADD_ASSIGN(loc);
"-="            return yy::parser::make_SUB_ASSIGN(loc);
"*="            return yy::parser::make_MUL_ASSIGN(loc);
"/="            return yy::parser::make_DIV_ASSIGN(loc);
"%="            return yy::parser::make_MOD_ASSIGN(loc);
"&="            return yy::parser::make_AND_ASSIGN(loc);
"^="            return yy::parser::make_XOR_ASSIGN(loc);
"|="            return yy::parser::make_OR_ASSIGN(loc);
">>"            return yy::parser::make_RIGHT_OP(loc);
"<<"            return yy::parser::make_LEFT_OP(loc);
"++"            return yy::parser::make_INC_OP(loc);
"--"            return yy::parser::make_DEC_OP(loc);
"->"            return yy::parser::make_PTR_OP(loc);
"&&"            return yy::parser::make_AND_OP(loc);
"||"            return yy::parser::make_OR_OP(loc);
"<="            return yy::parser::make_LE_OP(loc);
">="            return yy::parser::make_GE_OP(loc);
"=="            return yy::parser::make_EQ_OP(loc);
"!="            return yy::parser::make_NE_OP(loc);
";"             return yy::parser::make_SEMICOLON(loc);
"{"             return yy::parser::make_LBRACE(loc);
"}"             return yy::parser::make_RBRACE(loc);
","             return yy::parser::make_COMMA(loc);
":"             return yy::parser::make_COLON(loc);
"="             return yy::parser::make_ASSIGN(loc);
"("             return yy::parser::make_LPAREN(loc);
")"             return yy::parser::make_RPAREN(loc);
"["             return yy::parser::make_LBRACKET(loc);
"]"             return yy::parser::make_RBRACKET(loc);
"."             return yy::parser::make_DOT(loc);
"&"             return yy::parser::make_AMPERSAND(loc);
"!"             return yy::parser::make_EXCMARK(loc);
"~"             return yy::parser::make_TILDA(loc);
"-"             return yy::parser::make_MINUS(loc);
"+"             return yy::parser::make_PLUS(loc);
"*"             return yy::parser::make_ASTERISK(loc);
"/"             return yy::parser::make_SLASH(loc);
"%"             return yy::parser::make_PERCENT(loc);
"<"             return yy::parser::make_LANGLE(loc);
">"             return yy::parser::make_RANGLE(loc);
"^"             return yy::parser::make_CIRCUM(loc);
"|"             return yy::parser::make_VBAR(loc);
"?"             return yy::parser::make_QUESTMARK(loc);

[ \t\v\f]+      { loc.step(); }
\n+             { loc.lines(yyleng); loc.step(); }

.               {
                    char msg[64];
                    snprintf(msg, sizeof(msg),
                        "Unexpected symbol '%c' (0x%02hhX)\n",
                        yytext[0], yytext[0]);
                    msg[sizeof(msg) - 1] = 0;
                    drv.lexer_error(msg);
                }

<<EOF>>         return yy::parser::make_YYEOF(loc);

%%

void CoreDriver::scan_begin() {
    yylex_init(&scanner);
    yyset_debug(trace_scanning, scanner);
    yy_scan_string(text.c_str(), scanner); // TODO: avoid copying
//    bufState = yy_scan_string(text.c_str());
}

void CoreDriver::scan_end() {
//    yy_delete_buffer((YY_BUFFER_STATE)bufState);
    yylex_destroy(scanner);
}
