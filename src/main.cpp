#include <iostream>
#include <fstream>
#include <memory>
#include <chrono>
#include <fmt/core.h>
#include <fmt/chrono.h>

#include "cli_args.hpp"

#include "parser/preprocessor.hpp"
#include "parser/core_driver.hpp"
#include "ir/generator.hpp"

#include "transformations/vars_virtualizer.hpp"
#include "transformations/ssa_generator.hpp"
#include "transformations/computing_transformers.hpp"
#include "transformations/global_transformers.hpp"
#include "transformations/loop_inv_mover.hpp"
#include "transformations/intrinsics_detector.hpp"

#include "ir_2_llvm.hpp"

#include "subprocess.hpp"

using steady_clock = std::chrono::steady_clock;
using time_point = steady_clock::time_point;
using duration = steady_clock::duration;

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

static void writeOutBinary(std::string const &path, std::vector<char> const &data) {
    writeOutBinary(path, data.data(), data.size());
}

static void printElapsedTime(std::string const &name, duration const &time) {
    namespace chrono = std::chrono;
    auto ms = chrono::duration_cast<chrono::milliseconds>(time);
    auto us = chrono::duration_cast<chrono::microseconds>(time);
    fmt::print(stderr, "{:13s}: {:5} ({})\n", name, ms, us);
}


enum class CompilationLevel {
    PREPROCESS,     // files -> text
    PARSE,          // text -> ast
    GENERATE,       // ast -> ir (+optimization)
    MATERIALIZE,    // ir -> llvm
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

struct CompilationContext {
    CLIArgs const &args;
    CompilationLevel compilationLevel;

    std::string preprocessedText = "";
    std::shared_ptr<ParsingContext> parsingCtx;

    std::shared_ptr<AbstractSyntaxTree> ast;
    IntermediateUnit iunit;

    std::string llvmIR;
    std::vector<char> llvmBC;

    std::string assembly;

    CompilationContext(CLIArgs const &args) : args(args) {
        compilationLevel = getCompilationLvl(args);
    }
};

struct StepResult {
    duration elapsedTime;
    bool lastStep;
};


static StepResult doPreprocessing(CompilationContext &ctx) {
    time_point startTm = steady_clock::now();
    Preprocessor preproc(ctx.args.inputFile(), ctx.args.getDefines());
    time_point stopTm = steady_clock::now();

    std::tie(ctx.preprocessedText, ctx.parsingCtx) = std::move(preproc).moveData();

    if (ctx.args.outPreproc())
        writeOut(*ctx.args.outPreproc(), ctx.preprocessedText);
    return {
        .elapsedTime = stopTm - startTm,
        .lastStep = ctx.compilationLevel <= CompilationLevel::PREPROCESS,
    };
}

static StepResult doParsing(CompilationContext &ctx) {
    int parserDebugFlags = 0;
    if (ctx.args.isScannerTracing())
        parserDebugFlags |= CoreDriver::TRACE_SCANNER;
    if (ctx.args.isParserTracing())
        parserDebugFlags |= CoreDriver::TRACE_PARSER;

    std::string textCopy = ctx.preprocessedText;

    time_point startTm = steady_clock::now();
    CoreDriver parser(*ctx.parsingCtx, std::move(textCopy), parserDebugFlags);
    time_point stopTm = steady_clock::now();

    ctx.ast = std::move(parser).moveAST();

    if (ctx.args.outAST())
        writeOut(*ctx.args.outAST(), ctx.ast->top->getTreeNode(*ctx.parsingCtx)->printHor());
    return {
        .elapsedTime = stopTm - startTm,
        .lastStep = ctx.compilationLevel <= CompilationLevel::PARSE,
    };
}

static StepResult doIRGeneration(CompilationContext &ctx) {
    time_point startTm = steady_clock::now();
    IR_Generator gen(*ctx.ast, *ctx.parsingCtx);
    time_point stopTm = steady_clock::now();

    ctx.iunit = std::move(*std::move(gen).moveIR());

    return {
        .elapsedTime = stopTm - startTm,
        .lastStep = false, // There is also optimization step
    };
}


static void optimizeFunction(IntermediateUnit const &unit, IntermediateUnit::Function &func, CLIArgs const &args) {
    if (args.getOptLevel() == 0)
        return;

    CFGraph cfg = std::move(func.cfg);
    cfg = VarsVirtualizer(unit, std::move(cfg)).moveCfg();
    cfg = SSA_Generator(unit, std::move(cfg)).moveCfg();
    cfg = AlgebraicTransformer(unit, std::move(cfg)).moveCfg();
    cfg = CommonSubexprElim(unit, std::move(cfg)).moveCfg();
    cfg = CopyPropagator(unit, std::move(cfg)).moveCfg();
    cfg = TailrecEliminator(unit, std::move(cfg), func.getId()).moveCfg();

    if (args.getOptLevel() >= 2) {
        cfg = FunctionsInliner(unit, std::move(cfg)).moveCfg();
        cfg = LoopInvMover(unit, std::move(cfg)).moveCfg();
        if (args.isS1_Enabled()) {
            cfg = IntrinsicsDetector(unit, std::move(cfg)).moveCfg();
            cfg = CommonSubexprElim(unit, std::move(cfg)).moveCfg(); // TODO: temporary
        }
    }

    func.cfg = std::move(cfg);
}

static StepResult doOptimizations(CompilationContext &ctx) {
    time_point startTm = steady_clock::now();
    if (ctx.args.getOptLevel() != 0) {
        for (auto &[fId, func] : ctx.iunit.getFuncsMut()) {
            optimizeFunction(ctx.iunit, func, ctx.args);
        }
    }
    time_point stopTm = steady_clock::now();

    if (ctx.args.outIR())
        writeOut(*ctx.args.outIR(), ctx.iunit.printIR());
    if (ctx.args.outCFG())
        writeOut(*ctx.args.outCFG(), ctx.iunit.drawCFG());
    return {
        .elapsedTime = stopTm - startTm,
        .lastStep = ctx.compilationLevel <= CompilationLevel::GENERATE,
    };
}

static StepResult doMaterialization(CompilationContext &ctx) {
    time_point startTm = steady_clock::now();
    IR2LLVM materializer(ctx.iunit);
    time_point stopTm = steady_clock::now();

    std::tie(ctx.llvmIR, ctx.llvmBC) = std::move(materializer).moveData();

    if (ctx.args.outLLVM())
        writeOut(*ctx.args.outLLVM(), ctx.llvmIR);
    if (ctx.args.outBC())
        writeOutBinary(*ctx.args.outBC(), ctx.llvmBC);
    return {
        .elapsedTime = stopTm - startTm,
        .lastStep = ctx.compilationLevel <= CompilationLevel::MATERIALIZE,
    };
}

static StepResult doCompilation(CompilationContext &ctx) {
    namespace sp = subprocess;

    std::string llc_args = ctx.args.get_llc_name();
    if (ctx.args.getLLCArgs()) {
        llc_args += " " + *ctx.args.getLLCArgs(); // TODO: doesn't handle quoted args
    }
    else { // Default LLC arguments
        llc_args += " -O0 -mcpu=native";
    }

    time_point startTm = steady_clock::now();
    // TODO: handle exceptions here
    auto p = sp::Popen(llc_args, sp::input(sp::PIPE), sp::output(sp::PIPE));
    auto &llvmIr = ctx.llvmIR; // Can send bitcode
    auto res = p.communicate(llvmIr.c_str(), llvmIr.size());
    time_point stopTm = steady_clock::now();

    ctx.assembly = res.first.buf.data();

    if (ctx.args.outASM())
        writeOut(*ctx.args.outASM(), ctx.assembly);
    return {
        .elapsedTime = stopTm - startTm, // Times here are not so informative
        .lastStep = ctx.compilationLevel <= CompilationLevel::COMPILE,
    };
}


static void process(CLIArgs &args) {
    if (args.inputFile().empty())
        throw cw39_error("No input file");

    CompilationContext ctx(args);
    StepResult stepRes;

    // Preprocessor
    stepRes = doPreprocessing(ctx);
    if (args.isShowTimes())
        printElapsedTime("Preprocessor", stepRes.elapsedTime);
    if (stepRes.lastStep)
        return;

    // Parser
    stepRes = doParsing(ctx);
    if (args.isShowTimes())
        printElapsedTime("Parser", stepRes.elapsedTime);
    if (stepRes.lastStep)
        return;

    // IR generator
    stepRes = doIRGeneration(ctx);
    if (args.isShowTimes())
        printElapsedTime("Generator", stepRes.elapsedTime);

    // Optimizations (or nothig if -O0)
    stepRes = doOptimizations(ctx);
    if (args.isShowTimes())
        printElapsedTime("Optimizer", stepRes.elapsedTime);
    if (stepRes.lastStep)
        return;

    // Materializer
    stepRes = doMaterialization(ctx);
    if (args.isShowTimes())
        printElapsedTime("Materializer", stepRes.elapsedTime);
    if (stepRes.lastStep)
        return;

    // Compilation
    stepRes = doCompilation(ctx);
    if (args.isShowTimes())
        printElapsedTime("Compilation", stepRes.elapsedTime);
    if (stepRes.lastStep)
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
    catch (std::exception &exc) {
        std::cerr << "Something went wrong:\n\t" << exc.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
