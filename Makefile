EXEC_NAME=cw39
C_ARGS=-g -Wall -Wextra
CXX_ARGS=-std=c++20 -g -Wall -Wextra

# General

.PHONY: all clean clean_parser

all: $(EXEC_NAME)

include $(wildcard build/*.d)

OBJ_FILES= 											\
	build/main.o 									\
	build/parser/lexer.o build/parser/parser.o

$(EXEC_NAME): $(OBJ_FILES)
	$(info [*] Linking ...)
	g++ $(OBJ_FILES) $(CXX_ARGS) -o $(EXEC_NAME) -L. -lfmt

build/%.o: %.cpp
	$(info [*] Compiling (C++) $< ...)
	mkdir -p build
	g++ -MM $< > build/$(<:.cpp=.d)
	sed -i '1s:^:build/:' build/$(<:.cpp=.d)
	g++ $(CXX_ARGS) -c -o $@ $<

# Parser

build/parser/lexer.o: parser/lexer.c
	$(info [*] Compiling (C) $< ...)
	mkdir -p build/parser
	gcc $(C_ARGS) -c -o $@ $<

build/parser/parser.o: parser/parser.c
	$(info [*] Compiling (C) $< ...)
	mkdir -p build/parser
	gcc $(C_ARGS) -c -o $@ $<

parser/parser.c parser/y.tab.h: parser/parser.y
	$(info [*] Generating parser ...)
	yacc -Wall -Wno-yacc --defines=parser/y.tab.h -o parser/parser.c parser/parser.y

parser/lexer.c: parser/lexer.l parser/y.tab.h
	$(info [*] Generating lexer ...)
	lex -o parser/lexer.c parser/lexer.l

# Clean

clean:
	rm -f parser/lex.yy.c parser/y.tab.c parser/y.tab.h
	rm -f build/*.d build/*.o
	rm -f build/parser/*.o
	rm -df build/parser build
	rm -f a.out
