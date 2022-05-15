#include "common.hpp"
#include "lexer.hpp"
#include <fmt/core.h>
#include <cstdlib>
#include <cctype>
#include "core_driver.hpp"

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

AST_Literal_t get_integer(const char *str) {
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

AST_Literal_t get_float(const char *str, size_t len) {
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

AST_Literal_t get_charval(const char *str, size_t len) {
    AST_Literal res{
        .type = AST_Literal::CHARACTER,
        .longCnt = 0,
        .isUnsigned = 0,
        .isFloat = 0,
        .val = { 0ULL },
    };

    if (len == 3) {
        res.val.v_char = str[1];
    }
    else {
        if (str[1] != '\\' || len != 4)
            throw parser_exception(fmt::format("Wrong character literal: '{}'", str), "");
        res.val.v_char = unescapeChar(str[2]);
    }

    return res;
}
