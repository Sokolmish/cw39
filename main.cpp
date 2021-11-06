#include <iostream>

#include "parser/parser.hpp"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    auto test_prog =    
        "extern volatile unsigned long ((* const *var[4][restrict 5])[const])(int*[], double(*)[20], ...);\n"
		"static int x;";
    auto ast = parse_program(test_prog);
    printf("%lu\n", ast->children.size());
    delete ast;

    return 0;
}
