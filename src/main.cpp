#include <iostream>
#include <fstream>
#include <memory>
#include <fmt/core.h>

#include "cli_args.hpp"

#include "parser/preprocessor.hpp"
#include "parser/core_driver.hpp"
#include "ir/generator.hpp"

#include "transformations/vars_virtualizer.hpp"
#include "transformations/ssa_generator.hpp"
#include "transformations/algebraic_transformer.hpp"
#include "transformations/copy_propagator.hpp"
#include "transformations/tailrec_eliminator.hpp"

#include "ir_2_llvm.hpp"

/** If path is not empty then write to it, else write to stdout */
static void writeOut(std::string const &path, std::string const &data) {
    if (path.empty()) {
        fmt::print("{}\n", data);
    }
    else {
        std::ofstream myfile(path);
        myfile << data << std::endl;
    }
}

int main(int argc, char **argv) {
    CLIArgs args(argc, argv);

    if (args.unmatched().empty())
        generalError("No input files");
    std::string path = args.unmatched().at(0);

    Preprocessor preproc(path);
    std::string text = preproc.getText();
    auto warps = preproc.getWarps();

    if (args.count("preproc")) {
        writeOut(args.getString("preproc"), text);
    }

    auto parser = std::make_unique<CoreDriver>(text, warps);
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

    ControlFlowGraph optCfg;
    optCfg = VarsVirtualizer(*gen->getCfg()).moveCfg();
    optCfg = SSA_Generator(std::move(optCfg)).moveCfg();
    optCfg = AlgebraicTransformer(std::move(optCfg)).moveCfg();
    optCfg = CopyPropagator(std::move(optCfg)).moveCfg();
    optCfg = TailrecEliminator(std::move(optCfg)).moveCfg();

    if (args.count("ir"))
        writeOut(args.getString("ir"), optCfg.printIR());
    if (args.count("cfg"))
        writeOut(args.getString("cfg"), optCfg.drawCFG());

    if (args.count("llvm")) {
        IR2LLVM materializer(optCfg);
        writeOut(args.getString("llvm"), materializer.getRes());
    }

    return 0;
}
