#include "cli_args.hpp"
#include "cxxopts.hpp"
#include <fmt/core.h>

class CLIArgs::CLIArgs_Impl {
public:
    static constexpr const char *name = "cw39";
    static constexpr const char *vers = "0.2.0";
    static constexpr const char *desc =
            "An optimizing compiler for the subset of the C language.";

    cxxopts::Options options;
    cxxopts::ParseResult res;

    CLIArgs_Impl(int argc, char **argv);
};

CLIArgs::CLIArgs_Impl::CLIArgs_Impl(int argc, char **argv) : options(name, desc) {
    using namespace cxxopts;

    auto opt = options.add_options();

    opt("preproc", "Write preprocessor output", value<std::string>()->implicit_value(""));
    opt("ast", "Write AST", value<std::string>()->implicit_value(""));
    opt("ir-raw", "Write IR before optimizations", value<std::string>()->implicit_value(""));
    opt("cfg-raw", "Write CFG before optimizations", value<std::string>()->implicit_value(""));
    opt("ir", "Write IR after optimizations", value<std::string>()->implicit_value(""));
    opt("cfg", "Write CFG after optimizations", value<std::string>()->implicit_value(""));
    opt("llvm", "Write final LLVM", value<std::string>()->implicit_value(""));

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

std::vector<std::string> CLIArgs::unmatched() const {
    return impl->res.unmatched();
}

size_t CLIArgs::count(const std::string &name) const {
    return impl->res.count(name);
}

std::string CLIArgs::getString(const std::string &name) const {
    return impl->res[name].as<std::string>();
}
