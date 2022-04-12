#ifndef LEXER_HPP_INCLUDED__
#define LEXER_HPP_INCLUDED__

#include "common.hpp"

class CoreDriver;
void lexer_error(CoreDriver &drv, const char *msg);

AST_Literal_t get_integer(const char *str);
AST_Literal_t get_float(const char *str, size_t len);
AST_Literal_t get_charval(const char *str, size_t len);

// TODO: fix this
#define SL(node, loc) do { (node)->setLoc((loc)); } while (0)

#endif /* LEXER_HPP_INCLUDED__ */
