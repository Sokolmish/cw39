.PHONY: all clean

all: lex.yy.c y.tab.cpp y.tab.h

clean:
	rm -f lex.yy.c y.tab.cpp y.tab.h

y.tab.cpp y.tab.h: parser.y
	$(info [*] Generating parser ...)
	yacc -Wall -Wno-yacc --defines=y.tab.h -o y.tab.cpp parser.y

lex.yy.c: lexer.l y.tab.h
	$(info [*] Generating lexer ...)
	lex -o lex.yy.c lexer.l
