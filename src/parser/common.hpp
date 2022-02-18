#ifndef PARSER_COMMON_HPP_INCLUDED__
#define PARSER_COMMON_HPP_INCLUDED__

#include <string>
#include "common.h"

std::string get_ident_by_id(CoreParserState *rpstate, string_id_t id);
std::string get_string_by_id(CoreParserState *rpstate, string_id_t id);

#endif /* PARSER_COMMON_HPP_INCLUDED__ */
