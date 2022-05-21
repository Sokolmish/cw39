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
#include "transformations/intrinsics_detector.hpp"

#include "ir_2_llvm.hpp"
#include "ext_process.hpp"

/** If path is not empty then write to it, else write to stdout */
static void writeOut(std::string const &path, std::string const &data) {
    if (path.empty() || path == "-") {
        fmt::print("{}\n", data);
    }
    else {
        std::ofstream file(path);
        if (!file.is_open())
            throw cw39_error(fmt::format("Cannot open file '{}'", path));
        file << data << std::endl;
    }
}


static void writeOutBinary(std::string const &path, char const *data, size_t size) {
    if (path.empty() || path == "-") {
#ifdef __unix__
        if (path.empty() && isatty(fileno(stdout))) {
            throw cw39_error("Bitcode is not supposed to be written into terminal.\n"
                             "Use `--bc=-` to force this (can break something).");
        }
        FILE *out = fdopen(dup(fileno(stdout)), "wb");
        fwrite(data, 1, size, out); // TODO: missed check
        fclose(out);
#else
        throw cw39_error("Binary output into stdout is not available for current platform");
#endif
    }
    else {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open())
            throw cw39_error(fmt::format("Cannot open file '{}'", path));
        file.write(data, size);
    }
}

enum class CompilationLevel {
    PREPROCESS,     // files -> text
    PARSE,          // text -> ast
    GENERATE,       // ast -> ir (+optimization)
    MATERIALIZE,    // opt ir -> llvm
    COMPILE,        // llvm -> asm
};

static CompilationLevel getCompilationLvl(CLIArgs const &args) {
    if (args.outASM())
        return CompilationLevel::COMPILE;
    if (args.outLLVM() || args.outBC())
        return CompilationLevel::MATERIALIZE;
    else if (args.outIR() || args.outCFG())
        return CompilationLevel::GENERATE;
    else if (args.outAST())
        return CompilationLevel::PARSE;
    else if (args.outPreproc())
        return CompilationLevel::PREPROCESS;
    else
        return CompilationLevel::MATERIALIZE; // Default level
}

static void optimizeFunction(IntermediateUnit::Function &func, CLIArgs const &args) {
    if (args.getOptLevel() == 0)
        return;
    CFGraph cfg = std::move(func.cfg);
    cfg = VarsVirtualizer(std::move(cfg)).moveCfg();
    cfg = SSA_Generator(std::move(cfg)).moveCfg();
    cfg = AlgebraicTransformer(std::move(cfg)).moveCfg();
    cfg = CommonSubexprElim(std::move(cfg)).moveCfg();
    cfg = CopyPropagator(std::move(cfg)).moveCfg();
    cfg = TailrecEliminator(std::move(cfg), func.getId()).moveCfg();

    if (args.getOptLevel() >= 2) {
        cfg = LoopInvMover(std::move(cfg)).moveCfg();
        if (args.isS1_Enabled()) {
            cfg = IntrinsicsDetector(std::move(cfg)).moveCfg();
        }
    }

    func.cfg = std::move(cfg);
}

static void process(CLIArgs  &args) {
    auto compilationLvl = getCompilationLvl(args);

    if (args.inputFile().empty())
        throw cw39_error("No input file");
    std::string path = args.inputFile();

    Preprocessor preproc(path, args.getDefines());
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
        writeOut(*args.outAST(), ast->top->getTreeNode(*ctx)->printHor());
    }

    if (compilationLvl <= CompilationLevel::PARSE)
        return;

    auto gen = std::make_unique<IR_Generator>(*ast, *ctx);
    auto rawUnit = gen->getIR();

    IntermediateUnit optUnit = *rawUnit;
    for (auto &[fId, func] : optUnit.getFuncsMut()) {
        optimizeFunction(func, args.getOptLevel());
    }

    if (args.outIR())
        writeOut(*args.outIR(), optUnit.printIR());
    if (args.outCFG())
        writeOut(*args.outCFG(), optUnit.drawCFG());

    if (compilationLvl <= CompilationLevel::GENERATE)
        return;

    IR2LLVM materializer(optUnit);
    if (args.outLLVM()) {
        writeOut(*args.outLLVM(), materializer.getLLVM_IR());
    }
    if (args.outBC()) {
        auto const &data = materializer.getLLVM_BC();
        writeOutBinary(*args.outBC(), data.data(), data.size());
    }

    if (compilationLvl <= CompilationLevel::MATERIALIZE)
        return;

    if (args.outASM()) {
        ExtProcess llc_proc(args.get_llc_name(), {}); // TODO: catch output
        llc_proc.sendString(materializer.getLLVM_IR()); // Can send bitcode
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
