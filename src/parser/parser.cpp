#include "common.hpp"
#include "lexer.h"
#include "parser.hpp"
#include <fmt/core.h>

#include <cstdlib>
#include <cctype>

#include "y.tab.h"

CoreParser::CoreParser(const std::string &str, LinesWarpMap &warps) {
    pstate = std::make_unique<CoreParserState>();
    AST_TranslationUnit *rawUnit = CoreParser::parse_program(str, pstate.get(), &warps);
    unit = std::shared_ptr<AST_TranslationUnit>(rawUnit);
}

std::shared_ptr<AST_TranslationUnit> CoreParser::getTransUnit() {
    return unit;
}

CoreParserState *CoreParser::getPState() {
    return pstate.get();
}


void lexer_error(YYLTYPE *loc, lex_extra_t *extra, const char *msg) {
    auto fixLoc = extra->warps->getLoc(loc->first_line);
    std::string filename = extra->warps->getFilename(fixLoc.filenum);
    fmt::print(stderr, "Lexer error ({}:{}:{}):\n\t{}\n",
               filename, fixLoc.line, loc->first_column, msg);
}

void yyerror(void *loc, yyscan_t scan, AST_TranslationUnit **root, const LinesWarpMap *warps, const char *str) {
    (void)root;
    YYLTYPE *mloc = reinterpret_cast<YYLTYPE*>(loc);
    //fprintf(stderr, "error (%d:%d): %s\n", mloc->first_line, mloc->first_column, str);
    auto fixLoc = warps->getLoc(mloc->first_line);
    std::string filename = warps->getFilename(fixLoc.filenum);
    fmt::print(stderr, "Parser error ({}:{}:{}):\n\t{}\n",
               filename, fixLoc.line, mloc->first_column, str);
}


string_id_t get_ident_id(struct CoreParserState *pstate, const char *ident, int *type) {
    auto it = pstate->identsMap.lower_bound(ident);
    if (it == pstate->identsMap.end() || it->first != ident) {
        auto ins = pstate->identsMap.emplace_hint(it, ident, CoreParserState::IdentInfo(pstate->idCnt));
        pstate->invIdentsMap.emplace_hint(pstate->invIdentsMap.end(), pstate->idCnt, ins->first);
        pstate->idCnt++;
        *type = IDENTIFIER;
        return ins->second.id;
    }
    else { // If already exists
        *type = it->second.type == CoreParserState::IdentInfo::IDENT ? IDENTIFIER : TYPE_NAME;
        return it->second.id;
    }
}

static char unescapeChar(char ch) {
    // TODO: '\xhh'
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

string_id_t get_string_id(struct CoreParserState *pstate, const char *str) {
    size_t len = strlen(str);
    std::string newStr;
    newStr.reserve(len);

    for (size_t i = 1; i < len - 1; i++) {
        if (str[i] != '\\') {
            newStr.push_back(str[i]);
        }
        else {
            i++;
            if (i < len - 1)
                newStr.push_back(unescapeChar(str[i]));
        }
    }

    auto it = pstate->stringsMap.lower_bound(newStr);
    if (it == pstate->stringsMap.end() || it->first != newStr) {
        auto ins = pstate->stringsMap.emplace_hint(it, newStr, CoreParserState::StringInfo(pstate->strCnt));
        pstate->invStringsMap.emplace_hint(pstate->invStringsMap.end(), pstate->strCnt, ins->first);
        pstate->strCnt++;
        return ins->second.id;
    }
    else { // If already exists
        return it->second.id;
    }
}

std::string get_ident_by_id(CoreParserState *pstate, string_id_t id) {
    return std::string(pstate->invIdentsMap.find(id)->second);
}

std::string get_string_by_id(CoreParserState *pstate, string_id_t id) {
    return std::string(pstate->invStringsMap.find(id)->second);
}

void check_typedef(AST_Declaration *decl) {
    (void)decl; // TODO
}

AST_TypeName* get_def_type(string_id_t id) {
    (void)id;
    throw; // TODO
}


enum isuff {
    ISUFF_ERR, ISUFF_U, ISUFF_L, ISUFF_LL
};

static enum isuff get_int_suff(const char **str) {
    if (tolower((*str)[0]) == 'u') {
        (*str)++;
        return ISUFF_U;
    }
    else if (tolower((*str)[0]) == 'l') {
        (*str)++;
        if ((*str)[1] == (*str)[0]) {
            (*str)++;
            return ISUFF_LL;
        }
        return ISUFF_L;
    }
    else {
        return ISUFF_ERR;
    }
}

AST_Literal_t get_integer(const char *str) {
    const char *endptr, *suff;
    uint64_t val = strtoull(str, (char**)(&endptr), 0);

    AST_Literal res{
        .type = INTEGER_LITERAL,
        .longCnt = 0,
        .isUnsigned = 0,
        .isFloat = 0,
        .val = { 0ULL },
    };
    
    suff = endptr;
    while (*endptr) {
        enum isuff sf = get_int_suff(&endptr);
        if (sf == ISUFF_U && !res.isUnsigned)
            res.isUnsigned = 1;
        else if (sf == ISUFF_L && !res.longCnt)
            res.longCnt = 1;
        else if (sf == ISUFF_LL && !res.longCnt)
            res.longCnt = 2;
        else {
            fprintf(stderr, "Bad integer suffix: %s\n", suff);
            exit(EXIT_FAILURE); // TODO: error
        }
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

AST_Literal_t get_float(const char *str) {
    AST_Literal res{
        .type = FLOAT_LITERAL,
        .longCnt = 0,
        .isUnsigned = 0,
        .isFloat = 0,
        .val = { 0ULL },
    };

    size_t len = strlen(str);
    char *tmpStr = strdup(str);

    if (tolower(tmpStr[len - 1]) == 'f') {
        res.isFloat = 1;
        tmpStr[len - 1] = '\0';
    }
    else if (tolower(tmpStr[len - 1]) == 'l') {
        free(tmpStr);
        fprintf(stderr, "Long double is not implemented\n");
        exit(EXIT_FAILURE); // TODO: error
    }

    char *endptr;
    if (res.isFloat)
        res.val.vf32 = strtof(tmpStr, &endptr);
    else
        res.val.vf64 = strtod(tmpStr, &endptr);
    if (*endptr != '\0') {
        free(tmpStr);
        fprintf(stderr, "Wrong floating-point literal\n");
        exit(EXIT_FAILURE); // TODO: error
    }

    free(tmpStr);
    return res;
}

AST_Literal_t get_charval(const char *str) {
    AST_Literal res{
        .type = CHAR_LITERAL,
        .longCnt = 0,
        .isUnsigned = 0,
        .isFloat = 0,
        .val = { 0ULL },
    };

    size_t len = strlen(str);

    if (len == 3) {
        res.val.v_char = str[1];
    }
    else {
        if (str[1] != '\\' || len != 4) {
            fprintf(stderr, "Wrong character literal\n");
            exit(EXIT_FAILURE); // TODO: error
        }
        res.val.v_char = unescapeChar(str[2]);
    }

    return res;
}