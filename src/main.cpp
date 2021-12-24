#include <iostream>
#include <fstream>
#include <memory>
#include <fmt/core.h>

#include "parser/parser.hpp"
#include "ir/generator.hpp"

#include "transformations/vars_virtualizer.hpp"
#include "transformations/ssa_generator.hpp"
#include "transformations/algebraic_transformer.hpp"
#include "transformations/copy_propagator.hpp"

#include "ir_2_llvm.hpp"

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

    auto gen = std::make_unique<IR_Generator>();
    gen->parseAST(ast);

    auto cfg2 = VarsVirtualizer(*gen->getCfg()).getCfg();
    auto cfg3 = SSA_Generator(cfg2).getCfg();
    auto cfg4 = AlgebraicTransformer(cfg3).getCfg();
    auto cfg5 = CopyPropagator(cfg4).getCfg();
    cfg5->printCFG();

    fmt::print("\n================\n");

    IR2LLVM materializer(cfg5);
    std::cout << materializer.getRes() << std::endl;

    return 0;
}
