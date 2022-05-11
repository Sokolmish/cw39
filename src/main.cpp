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
#include "transformations/common_subexpr_elim.hpp"
#include "transformations/copy_propagator.hpp"
#include "transformations/tailrec_eliminator.hpp"
#include "transformations/loop_inv_mover.hpp"

#include "ir_2_llvm.hpp"
#include "ext_process.hpp"

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
    PREPROCESS,     // files -> text
    PARSE,          // text -> ast
    GENERATE,       // ast -> ir
    OPTIMIZE,       // ir -> opt ir
    MATERIALIZE,    // opt ir -> llvm
    COMPILE,        // llvm -> asm
};

static CompilationLevel getCompilationLvl(CLIArgs const &args) {
    if (args.outASM())
        return CompilationLevel::COMPILE;
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
    cfg = CommonSubexprElim(std::move(cfg)).moveCfg();
    cfg = CopyPropagator(std::move(cfg)).moveCfg();
    cfg = TailrecEliminator(std::move(cfg), func.getId()).moveCfg();
    cfg = LoopInvMover(std::move(cfg)).moveCfg();
    func.cfg = std::move(cfg);
}

static void process(CLIArgs  &args) {
    auto compilationLvl = getCompilationLvl(args);
    if (compilationLvl == CompilationLevel::NONE)
        return;

    if (args.inputFiles().empty())
        throw cw39_error("No input files");
    if (args.inputFiles().size() > 1)
        throw cw39_error("Cannot compile more than 1 file");
    std::string path = args.inputFiles()[0];

    Preprocessor preproc(path);
    std::string text = preproc.getText();
    auto ctx = preproc.getContext();

    if (args.outPreproc()) {
        writeOut(*args.outPreproc(), text);
    }

    if (compilationLvl <= CompilationLevel::PREPROCESS)
        return;

    int parserDebugFlags = 0;
    if (args.isScannerTracing())
        parserDebugFlags |= CoreDriver::TRACE_SCANNER;
    if (args.isParserTracing())
        parserDebugFlags |= CoreDriver::TRACE_PARSER;
    auto parser = std::make_unique<CoreDriver>(*ctx, text, parserDebugFlags);
    std::shared_ptr<AbstractSyntaxTree> ast = parser->getAST();
    if (args.outAST()) {
        ast_set_pctx_ptr(ctx.get());
        writeOut(*args.outAST(), ast->top->getTreeNode()->printHor());
        ast_set_pctx_ptr(nullptr);
    }

    if (compilationLvl <= CompilationLevel::PARSE)
        return;

    auto gen = std::make_unique<IR_Generator>(*ast, *ctx);
    auto rawUnit = gen->getIR();
    if (args.outRawIR())
        writeOut(*args.outRawIR(), rawUnit->printIR());
    if (args.outRawCFG())
        writeOut(*args.outRawCFG(), rawUnit->drawCFG());

    if (compilationLvl <= CompilationLevel::GENERATE)
        return;

    IntermediateUnit optUnit = *rawUnit;
    for (auto &[fId, func] : optUnit.getFuncsMut()) {
        optimizeFunction(func);
    }

    if (args.outIR())
        writeOut(*args.outIR(), optUnit.printIR());
    if (args.outCFG())
        writeOut(*args.outCFG(), optUnit.drawCFG());

    if (compilationLvl <= CompilationLevel::OPTIMIZE)
        return;

    IR2LLVM materializer(optUnit);
    if (args.outLLVM()) {
        writeOut(*args.outLLVM(), materializer.getRes());
    }

    if (compilationLvl <= CompilationLevel::MATERIALIZE)
        return;

    if (args.outASM()) {
        ExtProcess llc_proc(args.get_llc_name(), {}); // TODO: catch output
        llc_proc.sendString(materializer.getRes());
        llc_proc.closeFd();
        llc_proc.wait();
    }

    if (compilationLvl <= CompilationLevel::COMPILE)
        return;
}

int main(int argc, char **argv) {
    CLIArgs args(argc, argv);
    try {
        process(args);
    }
    catch (cw39_exception &exc) {
        std::cerr << exc.prettyWhat();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
