%option reentrant
%option nodefault
%option warn
%option noyywrap nounput noinput
%option batch
%option debug
%option never-interactive
%option ecs
%option fast
%option prefix="yycore"

%{
    #include <cstdlib>
    #include <string>

    #include "core_driver.hpp"
    #include "common.hpp"
    #include "yy_parser.hpp"

    #include "keywords_hashtable.cpp"

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
    yy::location &loc = drv.location;
    // Code run each time yylex is called.
    loc.step();
%}

{L}({L}|{D})*               {
                                auto kw = CoreKeywords::lookup(yytext, yyleng);
                                if (kw)
                                    return yy::parser::symbol_type(kw->id, loc);
                                string_id_t id = drv.ctx.getIdentId(yytext, yyleng);
                                if (drv.isDefinedType(id))
                                    return yy::parser::make_TYPE_NAME(id, loc);
                                else
                                    return yy::parser::make_IDENTIFIER(id, loc);
                            }

0[xX]{H}+{ISF}?             return yy::parser::make_CONSTANT(drv.get_integer(yytext), loc);
0[0-7]*{ISF}?               return yy::parser::make_CONSTANT(drv.get_integer(yytext), loc);
[1-9]{D}*{ISF}?             return yy::parser::make_CONSTANT(drv.get_integer(yytext), loc);

{D}+{EXP}{FSF}?             return yy::parser::make_CONSTANT(drv.get_float(yytext, yyleng), loc);
{D}*"."{D}+{EXP}?{FSF}?     return yy::parser::make_CONSTANT(drv.get_float(yytext, yyleng), loc);
{D}+"."{D}*{EXP}?{FSF}?     return yy::parser::make_CONSTANT(drv.get_float(yytext, yyleng), loc);

'(\\x..|\\.|[^\\'\n])'      return yy::parser::make_CONSTANT(drv.get_charval(yytext, yyleng), loc);

\"(\\.|[^\\"\n])*\"         {
                                string_id_t id = drv.ctx.getStringId(yytext, yyleng, true);
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
                    char ch = yytext[0];
                    drv.lexer_error(fmt::format("Unexpected symbol '{:c}' (0x{:02X})", ch, ch));
                }

<<EOF>>         return yy::parser::make_YYEOF(loc);

%%

void CoreDriver::scan_begin() {
    yylex_init(&scanner);
    yyset_debug(trace_scanning, scanner);
    yy_scan_buffer(text.data(), text.size(), scanner);
//    yy_scan_string(text.c_str(), scanner);
//    bufState = yy_scan_string(text.c_str());
}

void CoreDriver::scan_end() {
//    yy_delete_buffer((YY_BUFFER_STATE)bufState);
    yylex_destroy(scanner);
}


enum class IntSuff {
    ERR, U, L, LL
};

static IntSuff get_int_suff(const char **str) {
    if (tolower((*str)[0]) == 'u') {
        (*str)++;
        return IntSuff::U;
    }
    else if (tolower((*str)[0]) == 'l') {
        (*str)++;
        if ((*str)[1] == (*str)[0]) {
            (*str)++;
            return IntSuff::LL;
        }
        return IntSuff::L;
    }
    else {
        return IntSuff::ERR;
    }
}

AST_Literal_t CoreDriver::get_integer(const char *str) {
    const char *endptr, *suff;
    uint64_t val = strtoull(str, (char**)(&endptr), 0);

    AST_Literal res{
        .type = AST_Literal::INTEGER,
        .longCnt = 0,
        .isUnsigned = 0,
        .isFloat = 0,
        .val = { 0ULL },
    };

    suff = endptr;
    while (*endptr) {
        enum IntSuff sf = get_int_suff(&endptr);
        if (sf == IntSuff::U && !res.isUnsigned)
            res.isUnsigned = 1;
        else if (sf == IntSuff::L && !res.longCnt)
            res.longCnt = 1;
        else if (sf == IntSuff::LL && !res.longCnt)
            res.longCnt = 2;
        else
            throw parser_exception(fmt::format("Bad integer suffix: {}", suff), "");
    }

    // TODO: overflow

    if (res.isUnsigned) {
        if (res.longCnt)
            res.val.vu64 = static_cast<uint64_t>(val);
        else
            res.val.vu32 = static_cast<uint32_t>(val);
    }
    else {
        if (res.longCnt)
            res.val.vi64 = static_cast<int64_t>(val);
        else
            res.val.vi32 = static_cast<int32_t>(val);
    }
    return res;
}

AST_Literal_t CoreDriver::get_float(const char *str, size_t len) {
    AST_Literal res{
        .type = AST_Literal::FLOAT,
        .longCnt = 0,
        .isUnsigned = 0,
        .isFloat = 0,
        .val = { 0ULL },
    };

    std::string tmpStr(str);
    if (tolower(tmpStr[len - 1]) == 'f') {
        res.isFloat = 1;
        tmpStr.pop_back();
//        tmpStr[len - 1] = '\0';
    }
    else if (tolower(tmpStr[len - 1]) == 'l') {
        throw parser_exception("Long double is not supported", "");
    }

    char *endptr = nullptr;
    if (res.isFloat)
        res.val.vf32 = strtof(tmpStr.c_str(), &endptr);
    else
        res.val.vf64 = strtod(tmpStr.c_str(), &endptr);
    if (*endptr != '\0') {
        throw parser_exception(fmt::format("Wrong floating-point literal: '{}'", tmpStr), "");
    }

    return res;
}

AST_Literal_t CoreDriver::get_charval(const char *str, size_t len) {
    AST_Literal res{
        .type = AST_Literal::CHARACTER,
        .longCnt = 0,
        .isUnsigned = 0,
        .isFloat = 0,
        .val = { 0ULL },
    };

    if (len == 3) {
        if (str[1] == '\\')
            throw parser_exception(fmt::format("Wrong character literal: '{}'", str), "");
        res.val.v_char = str[1];
    }
    else {
        if (str[1] != '\\' || !(len == 4 || len == 6))
            throw parser_exception(fmt::format("Wrong character literal: '{}'", str), "");
        if (len == 4) {
            res.val.v_char = unescapeChar(str[2]);
        }
        else {
            if (str[2] != 'x' || !isxdigit(str[3]) || !isxdigit(str[4]))
                throw parser_exception(fmt::format("Wrong character literal: '{}'", str), "");
            res.val.v_char = parseXEscape(str[3], str[4]);
        }
    }

    return res;
}
