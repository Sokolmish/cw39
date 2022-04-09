/* C++ code produced by gperf version 3.1 */
/* Command-line: gperf -m 20 --output-file=directives_hashtable.cpp directives.gperf  */
/* Computed positions: -k'1-2' */

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

#line 6 "directives.gperf"

enum pc_directive_num {
    PC_DIR_INCLUDE,
    PC_DIR_DEFINE,
    PC_DIR_UNDEF,
    PC_DIR_IFDEF,
    PC_DIR_IFNDEF,
    PC_DIR_ENDIF,
    PC_DIR_ELSE,
    PC_DIR_LINE,
    PC_DIR_ERROR,
    PC_DIR_WARNING,
};
#line 23 "directives.gperf"
struct pc_directive { const char *name; pc_directive_num id; };
#include <string.h>
/* maximum key range = 10, duplicates = 0 */

class PC_Directives
{
private:
  static inline unsigned int hash (const char *str, size_t len);
public:
  static const struct pc_directive *lookup (const char *str, size_t len);
};

inline unsigned int
PC_Directives::hash (const char *str, size_t len)
{
  static const unsigned char asso_values[] =
    {
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14,  3, 14, 14,
       6,  0,  0, 14, 14,  2, 14, 14,  0, 14,
       0, 14, 14, 14,  6, 14, 14,  5, 14,  3,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14
    };
  return len + asso_values[static_cast<unsigned char>(str[1])] + asso_values[static_cast<unsigned char>(str[0])];
}

const struct pc_directive *
PC_Directives::lookup (const char *str, size_t len)
{
  enum
    {
      TOTAL_KEYWORDS = 10,
      MIN_WORD_LENGTH = 4,
      MAX_WORD_LENGTH = 7,
      MIN_HASH_VALUE = 4,
      MAX_HASH_VALUE = 13
    };

  static const struct pc_directive wordlist[] =
    {
#line 31 "directives.gperf"
      {"else",       PC_DIR_ELSE},
#line 30 "directives.gperf"
      {"endif",      PC_DIR_ENDIF},
#line 32 "directives.gperf"
      {"line",       PC_DIR_LINE},
#line 28 "directives.gperf"
      {"ifdef",      PC_DIR_IFDEF},
#line 29 "directives.gperf"
      {"ifndef",     PC_DIR_IFNDEF},
#line 25 "directives.gperf"
      {"include",    PC_DIR_INCLUDE},
#line 27 "directives.gperf"
      {"undef",      PC_DIR_UNDEF},
#line 33 "directives.gperf"
      {"error",      PC_DIR_ERROR},
#line 26 "directives.gperf"
      {"define",     PC_DIR_DEFINE},
#line 34 "directives.gperf"
      {"warning",    PC_DIR_WARNING}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= MIN_HASH_VALUE)
        {
          const struct pc_directive *resword;

          switch (key - 4)
            {
              case 0:
                resword = &wordlist[0];
                goto compare;
              case 1:
                resword = &wordlist[1];
                goto compare;
              case 2:
                resword = &wordlist[2];
                goto compare;
              case 3:
                resword = &wordlist[3];
                goto compare;
              case 4:
                resword = &wordlist[4];
                goto compare;
              case 5:
                resword = &wordlist[5];
                goto compare;
              case 6:
                resword = &wordlist[6];
                goto compare;
              case 7:
                resword = &wordlist[7];
                goto compare;
              case 8:
                resword = &wordlist[8];
                goto compare;
              case 9:
                resword = &wordlist[9];
                goto compare;
            }
          return 0;
        compare:
          {
            const char *s = resword->name;

            if (*str == *s && !strcmp (str + 1, s + 1))
              return resword;
          }
        }
    }
  return 0;
}
