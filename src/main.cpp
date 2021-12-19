#include <iostream>
#include <fstream>
#include <memory>
#include <fmt/core.h>

#include "parser/parser.hpp"
#include "ir/generator.hpp"

#include "transformations/vars_virtualizer.hpp"
#include "transformations/ssa_generator.hpp"

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

//    fmt::print("{}\n", ast->getTreeNode()->printHor());

    auto gen = std::make_unique<IR_Generator>();
    gen->parseAST(ast);
//    gen->getCfg()->printBlocks();

    auto cfg2 = VarsVirtualizer(*gen->getCfg()).getCfg();
    cfg2->printBlocks();

    //    Dominators dom(cfg2);
    //    std::cout << dom.drawGraph() << std::endl;

    fmt::print("\n================\n");

    auto cfg3 = SSA_Generator(cfg2).getCfg();
    cfg3->printBlocks();

    return 0;
}
