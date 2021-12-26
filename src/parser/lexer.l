%option reentrant noyywrap bison-bridge bison-locations
%option extra-type="struct lex_extra*"

%{
    #include "lexer.h"
    #include "y.tab.h"

    void comment(yyscan_t yyscanner);
%}

D           [0-9]
L           [a-zA-Z_]
H           [a-fA-F0-9]
EXP         ([Ee][+-]?{D}+)
FSF         (f|F|l|L)
ISF         ((u|U)|(u|U)?(l|L|ll|LL)|(l|L|ll|LL)(u|U))

%%

"/*"            { comment(yyscanner); }
"//"[^\n]*      { }

"auto"          { return AUTO;      }
"break"         { return BREAK;     }
"case"          { return CASE;      }
"char"          { return CHAR;      }
"const"         { return CONST;     }
"continue"      { return CONTINUE;  }
"default"       { return DEFAULT;   }
"do"            { return DO;        }
"double"        { return DOUBLE;    }
"else"          { return ELSE;      }
"enum"          { return ENUM;      }
"extern"        { return EXTERN;    }
"float"         { return FLOAT;     }
"for"           { return FOR;       }
"goto"          { return GOTO;      }
"if"            { return IF;        }
"inline"        { return INLINE;    }
"int"           { return INT;       }
"long"          { return LONG;      }
"register"      { return REGISTER;  }
(__)?"restrict" { return RESTRICT;  }
"return"        { return RETURN;    }
"short"         { return SHORT;     }
"signed"        { return SIGNED;    }
"sizeof"        { return SIZEOF;    }
"static"        { return STATIC;    }
"struct"        { return STRUCT;    }
"switch"        { return SWITCH;    }
"typedef"       { return TYPEDEF;   }
"union"         { return UNION;     }
"unsigned"      { return UNSIGNED;  }
"void"          { return VOID;      }
"volatile"      { return VOLATILE;  }
"while"         { return WHILE;     }

{L}({L}|{D})*		        { int type; yylval->ident_id = get_ident_id(yytext, &type); return type; }

0[xX]{H}+{ISF}?             { yylval->num_literal = get_integer(yytext); return(CONSTANT); }
0[0-7]*{ISF}?               { yylval->num_literal = get_integer(yytext); return(CONSTANT); }
[1-9]{D}*{ISF}?             { yylval->num_literal = get_integer(yytext); return(CONSTANT); }

{D}+{EXP}{FSF}?             { yylval->num_literal = get_float(yytext); return(CONSTANT); }
{D}*"."{D}+{EXP}?{FSF}?     { yylval->num_literal = get_float(yytext); return(CONSTANT); }
{D}+"."{D}*{EXP}?{FSF}?     { yylval->num_literal = get_float(yytext); return(CONSTANT); }

'(\\.|[^\\'\n])+'           { yylval->num_literal = get_charval(yytext); return(CONSTANT); }

\"(\\.|[^\\"\n])*\"         { yylval->str = get_string_id(yytext); return(STRING_LITERAL); }

"..."           { return ELLIPSIS;      }
">>="           { return RIGHT_ASSIGN;  }
"<<="           { return LEFT_ASSIGN;   }
"+="            { return ADD_ASSIGN;    }
"-="            { return SUB_ASSIGN;    }
"*="            { return MUL_ASSIGN;    }
"/="            { return DIV_ASSIGN;    }
"%="            { return MOD_ASSIGN;    }
"&="            { return AND_ASSIGN;    }
"^="            { return XOR_ASSIGN;    }
"|="            { return OR_ASSIGN;     }
">>"            { return RIGHT_OP;      }
"<<"            { return LEFT_OP;       }
"++"            { return INC_OP;        }
"--"            { return DEC_OP;        }
"->"            { return PTR_OP;        }
"&&"            { return AND_OP;        }
"||"            { return OR_OP;         }
"<="            { return LE_OP;         }
">="            { return GE_OP;         }
"=="            { return EQ_OP;         }
"!="            { return NE_OP;         }
";"             { return ';';   }
"{"             { return '{';   }
"}"             { return '}';   }
","             { return ',';   }
":"             { return ':';   }
"="             { return '=';   }
"("             { return '(';   }
")"             { return ')';   }
"["             { return '[';   }
"]"             { return ']';   }
"."			    { return '.';   }
"&"			    { return '&';   }
"!"			    { return '!';   }
"~"			    { return '~';   }
"-"			    { return '-';   }
"+"			    { return '+';   }
"*"			    { return '*';   }
"/"			    { return '/';   }
"%"			    { return '%';   }
"<"			    { return '<';   }
">"			    { return '>';   }
"^"			    { return '^';   }
"|"			    { return '|';   }
"?"             { return '?';   }

[ \t\v\n\f]     { }
.               { fprintf(stderr, "unexpected symbol\n"); exit(EXIT_FAILURE); }

%%

void comment(yyscan_t yyscanner) {
    char c, prev = 0;

    while ((c = input(yyscanner)) != 0) {
        if (c == '/' && prev == '*')
            return;
        prev = c;
    }
    fprintf(stderr, "unterminated comment\n"); // TODO: exit?
}


void init_scanner(const char *program, yyscan_t *scanner, lex_extra_t *extra) {
    extra->continued = 0;
    extra->cur_line = 0;
    extra->cur_column = 0;
    yylex_init_extra(extra, scanner);
    yy_scan_string(program, *scanner);
}

void destroy_scanner(yyscan_t scanner) {
    yylex_destroy(scanner);
}
