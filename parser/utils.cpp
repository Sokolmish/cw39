#include "common.h"
#include "parser.hpp"

#include <cstdlib>
#include <cstdint>
#include <cctype>

#include "y.tab.h"

std::map<std::string, IdentInfo, std::less<>> identifiers_map;
std::map<std::string, StringInfo, std::less<>> strings_map;

static std::map<string_id_t, std::string> inv_idents_map;
static std::map<string_id_t, std::string> inv_strings_map;

static string_id_t ident_cntr = 1;
static string_id_t str_cntr = 1;

string_id_t get_ident_id(const char *ident, int *type) {
    auto it = identifiers_map.lower_bound(ident);
    if (it == identifiers_map.end() || it->first != ident) {
        auto ins = identifiers_map.emplace_hint(it, ident, IdentInfo(ident_cntr));
        inv_idents_map.emplace_hint(inv_idents_map.end(), ident_cntr, ins->first);
        ident_cntr++;
        *type = IDENTIFIER;
        return ins->second.id;
    }
    else { // If already exists
        *type = it->second.type == IdentInfo::IDENT ? IDENTIFIER : TYPE_NAME;
        return it->second.id;
    }
}

string_id_t get_string_id(const char *str) {
    auto it = strings_map.lower_bound(str);
    if (it == strings_map.end() || it->first != str) {
        auto ins = strings_map.emplace_hint(it, str, StringInfo(str_cntr));
        inv_strings_map.emplace_hint(inv_strings_map.end(), ident_cntr, ins->first);
        str_cntr++;
        return ins->second.id;
    }
    else { // If already exists
        return it->second.id;
    }
}

std::string get_ident_by_id(string_id_t id) {
    return std::string(inv_idents_map.find(id)->second);
}

std::string get_string_by_id(string_id_t id) {
    return std::string(inv_strings_map.find(id)->second);
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

uint64_t get_integer(const char *str) {
    const char *endptr, *suff;
    uint64_t val = strtoull(str, (char**)(&endptr), 0);

    // TODO: overflow

    bool is_unsigned = false; // TODO: what to do with these flags?
    bool is_long = false;
    bool is_longlong = false;
    
    suff = endptr;
    while (*endptr) {
        enum isuff sf = get_int_suff(&endptr);
        if (sf == ISUFF_U && !is_unsigned)
            is_unsigned = true;
        else if (sf == ISUFF_L && !is_long && !is_longlong)
            is_long = true;
        else if (sf == ISUFF_LL && !is_long && !is_longlong)
            is_longlong = true;
        else {
            fprintf(stderr, "Bad integer suffix: %s\n", suff);
            exit(EXIT_FAILURE); // TODO: error
        }
    }

    return val;
}
