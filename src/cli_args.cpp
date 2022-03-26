#include "cli_args.hpp"
#include "cxxopts.hpp"
#include <fmt/core.h>

class CLIArgs::CLIArgs_Impl {
public:
    static constexpr const char *name = "cw39";
    static constexpr const char *desc = "C compiler";

    cxxopts::Options options;
    cxxopts::ParseResult res;

    CLIArgs_Impl(int argc, char **argv);
};

CLIArgs::CLIArgs_Impl::CLIArgs_Impl(int argc, char **argv) : options(name, desc) {
    using namespace cxxopts;

    options.add_options()
            ("preproc", "Write preprocessor outrrut", value<std::string>()->implicit_value(""))
            ("ast", "Write AST", value<std::string>()->implicit_value(""))
            ("ir-raw", "Write IR before optimizations", value<std::string>()->implicit_value(""))
            ("cfg-raw", "Write CFG before optimizations", value<std::string>()->implicit_value(""))
            ("ir", "Write IR after optimizations", value<std::string>()->implicit_value(""))
            ("cfg", "Write CFG after optimizations", value<std::string>()->implicit_value(""))
            ("llvm", "Write final LLVM", value<std::string>()->implicit_value(""))
            ("h,help", "Print usage");
    res = options.parse(argc, argv);

    if (res.count("help")) {
        fmt::print("{}\n", options.help());
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
