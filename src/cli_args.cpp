#include "cli_args.hpp"
#include "cxxopts.hpp"
#include <fmt/core.h>

class CLIArgs::CLIArgs_Impl {
public:
    static constexpr const char *name = "cw39";
    static constexpr const char *vers = "0.2.0";
    static constexpr const char *desc =
            "An optimizing compiler for the subset of the C language.";

    static constexpr const char *argPreproc = "preproc";
    static constexpr const char *argAst = "ast";
    static constexpr const char *argRawIr = "ir-raw";
    static constexpr const char *argRawCfg = "cfg-raw";
    static constexpr const char *argIr = "ir";
    static constexpr const char *argCfg = "cfg";
    static constexpr const char *argLLVM = "llvm";

    cxxopts::Options options;
    cxxopts::ParseResult res;

    CLIArgs_Impl(int argc, char **argv);
};

CLIArgs::CLIArgs_Impl::CLIArgs_Impl(int argc, char **argv) : options(name, desc) {
    using namespace cxxopts;

    auto opt = options.add_options();

    opt(argPreproc, "Write preprocessor output",
        value<std::string>()->implicit_value(""));
    opt(argAst, "Write AST",
        value<std::string>()->implicit_value(""));
    opt(argRawIr, "Write IR before optimizations",
        value<std::string>()->implicit_value(""));
    opt(argRawCfg, "Write CFG before optimizations",
        value<std::string>()->implicit_value(""));
    opt(argIr, "Write IR after optimizations",
        value<std::string>()->implicit_value(""));
    opt(argCfg, "Write CFG after optimizations",
        value<std::string>()->implicit_value(""));
    opt(argLLVM, "Write final LLVM",
        value<std::string>()->implicit_value(""));

    opt("version", "Print version");
    opt("h,help", "Print usage");

    try {
        res = options.parse(argc, argv);
    }
    catch (cxxopts::OptionException const &exc) {
        fmt::print(stderr, "Error: {}\n", exc.what());
        exit(EXIT_FAILURE);
    }

    if (res.count("help")) {
        fmt::print("{}\n", options.help());
        exit(EXIT_SUCCESS);
    }
    else if (res.count("version")) {
        fmt::print("{} version: {}\n", name, vers);
        exit(EXIT_SUCCESS);
    }
}

CLIArgs::CLIArgs(int argc, char **argv)
        : impl(std::make_unique<CLIArgs_Impl>(argc, argv)) {}

CLIArgs::~CLIArgs() = default;

std::string CLIArgs::getString(const std::string &name) const {
    return impl->res[name].as<std::string>();
}

std::optional<std::string> CLIArgs::getOutParam(const char *name) const {
    if (impl->res.count(name))
        return getString(name);
    return std::optional<std::string>();
}

std::optional<std::string> CLIArgs::outPreproc() const {
    return getOutParam(CLIArgs_Impl::argPreproc);
}

std::optional<std::string> CLIArgs::outAST() const {
    return getOutParam(CLIArgs_Impl::argAst);
}

std::optional<std::string> CLIArgs::outRawIR() const {
    return getOutParam(CLIArgs_Impl::argRawIr);
}

std::optional<std::string> CLIArgs::outRawCFG() const {
    return getOutParam(CLIArgs_Impl::argRawCfg);
}

std::optional<std::string> CLIArgs::outIR() const {
    return getOutParam(CLIArgs_Impl::argIr);
}

std::optional<std::string> CLIArgs::outCFG() const {
    return getOutParam(CLIArgs_Impl::argCfg);
}

std::optional<std::string> CLIArgs::outLLVM() const {
    return getOutParam(CLIArgs_Impl::argLLVM);
}

std::vector<std::string> CLIArgs::inputFiles() const {
    return impl->res.unmatched();
}
