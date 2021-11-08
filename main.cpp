#include <iostream>
#include <fstream>
#include <memory>

#include "parser/parser.hpp"

std::string readFile(std::string const &path) {
    std::ifstream t(path.c_str());
    t.seekg(0, std::ios::end);
    auto size = t.tellg();
    std::string buffer(size, ' ');
    t.seekg(0);
    t.read(&buffer[0], size);
    return buffer;
}

int main(int argc, char **argv) {
    std::string path = "tests/tst_prog1.c";
    if (argc > 1)
        path = argv[1];

    auto text = readFile(path);
    auto ast = std::shared_ptr<AST_TranslationUnit>(parse_program(text));

    std::cout << ast->getTreeNode()->printHor() << std::endl;

    return 0;
}
