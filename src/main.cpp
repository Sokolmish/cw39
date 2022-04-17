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

enum class CompilationLevel {
    NONE = 0,
    PREPROCESS,
    PARSE,
    GENERATE,
    OPTIMIZE,
    MATERIALIZE,
    EXTERNAL,
};

static CompilationLevel getCompilationLvl(CLIArgs const &args) {
    if (args.outLLVM())
        return CompilationLevel::MATERIALIZE;
    else if (args.outIR() || args.outCFG())
        return CompilationLevel::OPTIMIZE;
    else if (args.outRawIR() || args.outRawCFG())
        return CompilationLevel::GENERATE;
    else if (args.outAST())
        return CompilationLevel::PARSE;
    else if (args.outPreproc())
        return CompilationLevel::PREPROCESS;
    else
        return CompilationLevel::NONE;
}

static void optimizeFunction(IntermediateUnit::Function &func) {
    CFGraph cfg = std::move(func.cfg);
    cfg = VarsVirtualizer(std::move(cfg)).moveCfg();
    cfg = SSA_Generator(std::move(cfg)).moveCfg();
    cfg = AlgebraicTransformer(std::move(cfg)).moveCfg();
    cfg = CopyPropagator(std::move(cfg)).moveCfg();
    cfg = TailrecEliminator(std::move(cfg), func.getId()).moveCfg();
    func.cfg = std::move(cfg);
}

int main(int argc, char **argv) {
    CLIArgs args(argc, argv);

    auto compilationLvl = getCompilationLvl(args);
    if (compilationLvl == CompilationLevel::NONE)
        return 0;

    if (args.inputFiles().empty())
        generalError("No input files");
    if (args.inputFiles().size() > 1)
        generalError("Cannot compile more than 1 file");
    std::string path = args.inputFiles()[0];

    Preprocessor preproc(path);
    std::string text = preproc.getText();
    auto ctx = preproc.getContext();

    if (args.outPreproc()) {
        writeOut(*args.outPreproc(), text);
    }

    if (compilationLvl <= CompilationLevel::PREPROCESS)
        return 0;

    auto parser = std::make_unique<CoreDriver>(*ctx, text);
    auto ast = parser->getTransUnit();
    if (args.outAST()) {
        ast_set_pctx_ptr(ctx.get());
        writeOut(*args.outAST(), ast->getTreeNode()->printHor());
        ast_set_pctx_ptr(nullptr);
    }

    if (compilationLvl <= CompilationLevel::PARSE)
        return 0;

    auto gen = std::make_unique<IR_Generator>(*parser, *ctx);
    auto rawUnit = gen->getIR();
    if (args.outRawIR())
        writeOut(*args.outRawIR(), rawUnit->printIR());
    if (args.outRawCFG())
        writeOut(*args.outRawCFG(), rawUnit->drawCFG());

    if (compilationLvl <= CompilationLevel::GENERATE)
        return 0;

    IntermediateUnit optUnit = *rawUnit;
    for (auto &[fId, func] : optUnit.getFuncsMut()) {
        optimizeFunction(func);
    }

    if (args.outIR())
        writeOut(*args.outIR(), optUnit.printIR());
    if (args.outCFG())
        writeOut(*args.outCFG(), optUnit.drawCFG());

    if (compilationLvl <= CompilationLevel::OPTIMIZE)
        return 0;

    IR2LLVM materializer(optUnit);
    if (args.outLLVM()) {
        writeOut(*args.outLLVM(), materializer.getRes());
    }

    if (compilationLvl <= CompilationLevel::MATERIALIZE)
        return 0;

    return 0;
}
