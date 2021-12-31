#include <iostream>
#include <fstream>
#include <memory>
#include <fmt/core.h>

#include "cxxopts.hpp"

#include "parser/parser.hpp"
#include "ir/generator.hpp"

#include "transformations/vars_virtualizer.hpp"
#include "transformations/ssa_generator.hpp"
#include "transformations/algebraic_transformer.hpp"
#include "transformations/copy_propagator.hpp"

#include "ir_2_llvm.hpp"

static std::string readFile(std::string const &path) {
    std::ifstream t(path.c_str());
    t.seekg(0, std::ios::end);
    auto size = t.tellg();
    std::string buffer(size, ' ');
    t.seekg(0);
    t.read(&buffer[0], size);
    return buffer;
}

/** If path is not empty then write to it, else write to stdout */
static void writeOut(std::string const &path, std::string const &data) {
    if (path.empty()) {
        fmt::print("{}\n", data);
    }
    else {
        std::ofstream myfile;
        myfile.open(path);
        myfile << data << std::endl;
        myfile.close();
    }
}

int main(int argc, char **argv) {
    cxxopts::Options options("cw39", "C compiler");
    options.add_options()
            ("ast", "Write AST", cxxopts::value<std::string>()->implicit_value(""))
            ("ir-raw", "Write IR before optimizations", cxxopts::value<std::string>()->implicit_value(""))
            ("cfg-raw", "Write CFG before optimizations", cxxopts::value<std::string>()->implicit_value(""))
            ("ir", "Write IR after optimizations", cxxopts::value<std::string>()->implicit_value(""))
            ("cfg", "Write CFG after optimizations", cxxopts::value<std::string>()->implicit_value(""))
            ("llvm", "Write final LLVM", cxxopts::value<std::string>()->implicit_value(""))
            ("h,help", "Print usage");
    auto args = options.parse(argc, argv);
    if (args.count("help")) {
        fmt::print("{}\n", options.help());
        exit(EXIT_SUCCESS);
    }

    std::string path = "tests/tst_prog1.c";
    if (!args.unmatched().empty())
        path = args.unmatched().at(0);

    auto text = readFile(path);
    auto ast = std::shared_ptr<AST_TranslationUnit>(parse_program(text));

    if (args.count("ast"))
        writeOut(args["ast"].as<std::string>(), ast->getTreeNode()->printHor());

    auto gen = std::make_unique<IR_Generator>();
    gen->parseAST(ast);

    if (args.count("ir-raw"))
        writeOut(args["ir-raw"].as<std::string>(), gen->getCfg()->printIR());
    if (args.count("cfg-raw"))
        writeOut(args["cfg-raw"].as<std::string>(), gen->getCfg()->drawCFG());

    auto cfg2 = VarsVirtualizer(*gen->getCfg()).getCfg();
    auto cfg3 = SSA_Generator(cfg2).getCfg();
    auto cfg4 = AlgebraicTransformer(cfg3).getCfg();
    auto cfg5 = CopyPropagator(cfg4).getCfg();

    if (args.count("ir"))
        writeOut(args["ir"].as<std::string>(), cfg5->printIR());
    if (args.count("cfg"))
        writeOut(args["cfg"].as<std::string>(), cfg5->drawCFG());

    IR2LLVM materializer(cfg5);
    if (args.count("llvm"))
        writeOut(args["llvm"].as<std::string>(), materializer.getRes());

    return 0;
}
