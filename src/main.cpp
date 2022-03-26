#include <iostream>
#include <fstream>
#include <memory>
#include <fmt/core.h>

#include "cli_args.hpp"

#include "parser/preprocessor.hpp"
#include "parser/parser.hpp"
#include "ir/generator.hpp"

#include "transformations/vars_virtualizer.hpp"
#include "transformations/ssa_generator.hpp"
#include "transformations/algebraic_transformer.hpp"
#include "transformations/copy_propagator.hpp"

#include "ir_2_llvm.hpp"

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
    CLIArgs args(argc, argv);

    std::string path = "tests/tst_prog1.c";
    if (!args.unmatched().empty())
        path = args.unmatched().at(0);

    auto preproc = Preprocessor(path);
    std::string text = preproc.getText();
    auto warps = preproc.getWarps();

    if (args.count("preproc")) {
        writeOut(args.getString("preproc"), text);
    }

    auto parser = std::make_unique<CoreParser>(text, warps);
    auto ast = parser->getTransUnit();

    if (args.count("ast")) {
        ast_set_pstate_ptr(parser->getPState());
        writeOut(args.getString("ast"), ast->getTreeNode()->printHor());
        ast_set_pstate_ptr(nullptr);
    }

    auto gen = std::make_unique<IR_Generator>();
    gen->parse(*parser, warps);

    if (args.count("ir-raw"))
        writeOut(args.getString("ir-raw"), gen->getCfg()->printIR());
    if (args.count("cfg-raw"))
        writeOut(args.getString("cfg-raw"), gen->getCfg()->drawCFG());

    auto cfg2 = VarsVirtualizer(*gen->getCfg()).getCfg();
    auto cfg3 = SSA_Generator(cfg2).getCfg();
    auto cfg4 = AlgebraicTransformer(cfg3).getCfg();
    auto cfg5 = CopyPropagator(cfg4).getCfg();

    if (args.count("ir"))
        writeOut(args.getString("ir"), cfg5->printIR());
    if (args.count("cfg"))
        writeOut(args.getString("cfg"), cfg5->drawCFG());

    if (args.count("llvm")) {
        IR2LLVM materializer(cfg5);
        writeOut(args.getString("llvm"), materializer.getRes());
    }

    return 0;
}
