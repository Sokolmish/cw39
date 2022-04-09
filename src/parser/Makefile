.PHONY: all clean core_parser hashtables

all: core_parser hashtables

core_parser: lex.yy.c y.tab.cpp y.tab.h

hashtables: directives_hashtable.cpp

clean:
	rm -f lex.yy.c y.tab.cpp y.tab.h
	rm -f directives_hashtable.cpp

# Flex and Bison

y.tab.cpp y.tab.h: parser.y
	$(info [*] Generating parser ...)
	bison -Wall -Wno-yacc --defines=y.tab.h -o y.tab.cpp parser.y

lex.yy.c: lexer.l y.tab.h
	$(info [*] Generating lexer ...)
	flex -o lex.yy.c lexer.l

# Gperf

directives_hashtable.cpp: directives.gperf
	gperf -m 20 --output-file=$@ $<
