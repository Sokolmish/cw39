/* C++ code produced by gperf version 3.1 */
/* Command-line: gperf -m 20 --output-file=keywords_hashtable.cpp keywords.gperf  */
/* Computed positions: -k'1,3' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#line 10 "keywords.gperf"
struct core_keyword { const char *name; yy::parser::token_kind_type id; };
#include <string.h>
/* maximum key range = 42, duplicates = 0 */

class CoreKeywords
{
private:
  static inline unsigned int hash (const char *str, size_t len);
public:
  static const struct core_keyword *lookup (const char *str, size_t len);
};

inline unsigned int
CoreKeywords::hash (const char *str, size_t len)
{
  static const unsigned char asso_values[] =
    {
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 11, 48, 18, 24, 18,
      29, 14, 11,  9, 48,  4, 48, 48, 23, 48,
       9, 30, 28, 48,  3,  4,  3,  9,  3, 20,
      48, 48, 31, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
      48, 48, 48, 48, 48, 48
    };
  unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[static_cast<unsigned char>(str[2])];
      /*FALLTHROUGH*/
      case 2:
      case 1:
        hval += asso_values[static_cast<unsigned char>(str[0])];
        break;
    }
  return hval;
}

const struct core_keyword *
CoreKeywords::lookup (const char *str, size_t len)
{
  enum
    {
      TOTAL_KEYWORDS = 39,
      MIN_WORD_LENGTH = 2,
      MAX_WORD_LENGTH = 13,
      MIN_HASH_VALUE = 6,
      MAX_HASH_VALUE = 47
    };

  static const struct core_keyword wordlist[] =
    {
#line 27 "keywords.gperf"
      {"if",             yy::parser::token::CORE_TOK_IF},
#line 29 "keywords.gperf"
      {"int",            yy::parser::token::CORE_TOK_INT},
#line 44 "keywords.gperf"
      {"void",           yy::parser::token::CORE_TOK_VOID},
#line 34 "keywords.gperf"
      {"return",         yy::parser::token::CORE_TOK_RETURN},
#line 39 "keywords.gperf"
      {"struct",         yy::parser::token::CORE_TOK_STRUCT},
#line 40 "keywords.gperf"
      {"switch",         yy::parser::token::CORE_TOK_SWITCH},
#line 32 "keywords.gperf"
      {"restrict",       yy::parser::token::CORE_TOK_RESTRICT},
#line 26 "keywords.gperf"
      {"goto",           yy::parser::token::CORE_TOK_GOTO},
#line 25 "keywords.gperf"
      {"for",            yy::parser::token::CORE_TOK_FOR},
#line 42 "keywords.gperf"
      {"union",          yy::parser::token::CORE_TOK_UNION},
#line 36 "keywords.gperf"
      {"signed",         yy::parser::token::CORE_TOK_SIGNED},
#line 31 "keywords.gperf"
      {"register",       yy::parser::token::CORE_TOK_REGISTER},
#line 43 "keywords.gperf"
      {"unsigned",       yy::parser::token::CORE_TOK_UNSIGNED},
#line 21 "keywords.gperf"
      {"else",           yy::parser::token::CORE_TOK_ELSE},
#line 23 "keywords.gperf"
      {"extern",         yy::parser::token::CORE_TOK_EXTERN},
#line 33 "keywords.gperf"
      {"__restrict",     yy::parser::token::CORE_TOK_RESTRICT},
#line 12 "keywords.gperf"
      {"auto",           yy::parser::token::CORE_TOK_AUTO},
#line 14 "keywords.gperf"
      {"case",           yy::parser::token::CORE_TOK_CASE},
#line 22 "keywords.gperf"
      {"enum",           yy::parser::token::CORE_TOK_ENUM},
#line 38 "keywords.gperf"
      {"static",         yy::parser::token::CORE_TOK_STATIC},
#line 46 "keywords.gperf"
      {"while",          yy::parser::token::CORE_TOK_WHILE},
#line 48 "keywords.gperf"
      {"__fconst",       yy::parser::token::CORE_TOK_EXT_FCONST},
#line 19 "keywords.gperf"
      {"do",             yy::parser::token::CORE_TOK_DO},
#line 16 "keywords.gperf"
      {"const",          yy::parser::token::CORE_TOK_CONST},
#line 28 "keywords.gperf"
      {"inline",         yy::parser::token::CORE_TOK_INLINE},
#line 45 "keywords.gperf"
      {"volatile",       yy::parser::token::CORE_TOK_VOLATILE},
#line 17 "keywords.gperf"
      {"continue",       yy::parser::token::CORE_TOK_CONTINUE},
#line 30 "keywords.gperf"
      {"long",           yy::parser::token::CORE_TOK_LONG},
#line 50 "keywords.gperf"
      {"__weak",         yy::parser::token::CORE_TOK_EXT_WEAK},
#line 41 "keywords.gperf"
      {"typedef",        yy::parser::token::CORE_TOK_TYPEDEF},
#line 35 "keywords.gperf"
      {"short",          yy::parser::token::CORE_TOK_SHORT},
#line 15 "keywords.gperf"
      {"char",           yy::parser::token::CORE_TOK_CHAR},
#line 37 "keywords.gperf"
      {"sizeof",         yy::parser::token::CORE_TOK_SIZEOF},
#line 47 "keywords.gperf"
      {"__attribute__",  yy::parser::token::CORE_TOK_ATTRIBUTE},
#line 13 "keywords.gperf"
      {"break",          yy::parser::token::CORE_TOK_BREAK},
#line 20 "keywords.gperf"
      {"double",         yy::parser::token::CORE_TOK_DOUBLE},
#line 49 "keywords.gperf"
      {"__pure",         yy::parser::token::CORE_TOK_EXT_PURE},
#line 24 "keywords.gperf"
      {"float",          yy::parser::token::CORE_TOK_FLOAT},
#line 18 "keywords.gperf"
      {"default",        yy::parser::token::CORE_TOK_DEFAULT}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= MIN_HASH_VALUE)
        {
          const struct core_keyword *resword;

          switch (key - 6)
            {
              case 0:
                if (len == 2)
                  {
                    resword = &wordlist[0];
                    goto compare;
                  }
                break;
              case 4:
                if (len == 3)
                  {
                    resword = &wordlist[1];
                    goto compare;
                  }
                break;
              case 5:
                if (len == 4)
                  {
                    resword = &wordlist[2];
                    goto compare;
                  }
                break;
              case 6:
                if (len == 6)
                  {
                    resword = &wordlist[3];
                    goto compare;
                  }
                break;
              case 7:
                if (len == 6)
                  {
                    resword = &wordlist[4];
                    goto compare;
                  }
                break;
              case 8:
                if (len == 6)
                  {
                    resword = &wordlist[5];
                    goto compare;
                  }
                break;
              case 9:
                if (len == 8)
                  {
                    resword = &wordlist[6];
                    goto compare;
                  }
                break;
              case 10:
                if (len == 4)
                  {
                    resword = &wordlist[7];
                    goto compare;
                  }
                break;
              case 11:
                if (len == 3)
                  {
                    resword = &wordlist[8];
                    goto compare;
                  }
                break;
              case 12:
                if (len == 5)
                  {
                    resword = &wordlist[9];
                    goto compare;
                  }
                break;
              case 13:
                if (len == 6)
                  {
                    resword = &wordlist[10];
                    goto compare;
                  }
                break;
              case 14:
                if (len == 8)
                  {
                    resword = &wordlist[11];
                    goto compare;
                  }
                break;
              case 15:
                if (len == 8)
                  {
                    resword = &wordlist[12];
                    goto compare;
                  }
                break;
              case 16:
                if (len == 4)
                  {
                    resword = &wordlist[13];
                    goto compare;
                  }
                break;
              case 17:
                if (len == 6)
                  {
                    resword = &wordlist[14];
                    goto compare;
                  }
                break;
              case 18:
                if (len == 10)
                  {
                    resword = &wordlist[15];
                    goto compare;
                  }
                break;
              case 19:
                if (len == 4)
                  {
                    resword = &wordlist[16];
                    goto compare;
                  }
                break;
              case 20:
                if (len == 4)
                  {
                    resword = &wordlist[17];
                    goto compare;
                  }
                break;
              case 21:
                if (len == 4)
                  {
                    resword = &wordlist[18];
                    goto compare;
                  }
                break;
              case 22:
                if (len == 6)
                  {
                    resword = &wordlist[19];
                    goto compare;
                  }
                break;
              case 23:
                if (len == 5)
                  {
                    resword = &wordlist[20];
                    goto compare;
                  }
                break;
              case 24:
                if (len == 8)
                  {
                    resword = &wordlist[21];
                    goto compare;
                  }
                break;
              case 25:
                if (len == 2)
                  {
                    resword = &wordlist[22];
                    goto compare;
                  }
                break;
              case 26:
                if (len == 5)
                  {
                    resword = &wordlist[23];
                    goto compare;
                  }
                break;
              case 27:
                if (len == 6)
                  {
                    resword = &wordlist[24];
                    goto compare;
                  }
                break;
              case 28:
                if (len == 8)
                  {
                    resword = &wordlist[25];
                    goto compare;
                  }
                break;
              case 29:
                if (len == 8)
                  {
                    resword = &wordlist[26];
                    goto compare;
                  }
                break;
              case 30:
                if (len == 4)
                  {
                    resword = &wordlist[27];
                    goto compare;
                  }
                break;
              case 31:
                if (len == 6)
                  {
                    resword = &wordlist[28];
                    goto compare;
                  }
                break;
              case 32:
                if (len == 7)
                  {
                    resword = &wordlist[29];
                    goto compare;
                  }
                break;
              case 33:
                if (len == 5)
                  {
                    resword = &wordlist[30];
                    goto compare;
                  }
                break;
              case 34:
                if (len == 4)
                  {
                    resword = &wordlist[31];
                    goto compare;
                  }
                break;
              case 35:
                if (len == 6)
                  {
                    resword = &wordlist[32];
                    goto compare;
                  }
                break;
              case 36:
                if (len == 13)
                  {
                    resword = &wordlist[33];
                    goto compare;
                  }
                break;
              case 37:
                if (len == 5)
                  {
                    resword = &wordlist[34];
                    goto compare;
                  }
                break;
              case 38:
                if (len == 6)
                  {
                    resword = &wordlist[35];
                    goto compare;
                  }
                break;
              case 39:
                if (len == 6)
                  {
                    resword = &wordlist[36];
                    goto compare;
                  }
                break;
              case 40:
                if (len == 5)
                  {
                    resword = &wordlist[37];
                    goto compare;
                  }
                break;
              case 41:
                if (len == 7)
                  {
                    resword = &wordlist[38];
                    goto compare;
                  }
                break;
            }
          return 0;
        compare:
          {
            const char *s = resword->name;

            if (*str == *s && !memcmp (str + 1, s + 1, len - 1))
              return resword;
          }
        }
    }
  return 0;
}
