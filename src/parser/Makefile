.PHONY: all clean core_parser hashtables

all: core_parser hashtables

core_parser: yy_scanner.cpp yy_parser.cpp yy_parser.hpp

hashtables: directives_hashtable.cpp

clean:
	rm -f scanner.cpp parser.cpp parser.hpp
	rm -f directives_hashtable.cpp

# Flex and Bison

yy_parser.cpp yy_parser.hpp: parser.y
	$(info [*] Generating parser ...)
	bison -Wall --defines=yy_parser.hpp -Dapi.location.file="yy_location.hpp" -o yy_parser.cpp parser.y

yy_scanner.cpp: lexer.l
	$(info [*] Generating lexer ...)
	flex -o yy_scanner.cpp lexer.l

# Gperf

directives_hashtable.cpp: directives.gperf
	gperf -m 20 --output-file=$@ $<
