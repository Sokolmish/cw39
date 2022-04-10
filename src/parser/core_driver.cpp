#include "core_driver.hpp"
#include <fmt/core.h>

CoreDriver::CoreDriver(std::string program, LinesWarpMap const &linesWarps)
        : text(std::move(program)), warps(linesWarps) {
    trace_parsing = false;
    trace_scanning = false;

    pstate = std::make_unique<CoreParserState>();

    parse();
}

void CoreDriver::parse() {
    location.initialize(nullptr);
    scan_begin();
    yy::parser parse(scanner, *this);
    parse.set_debug_level(trace_parsing);
    int rc = parse();
    scan_end();

    if (rc != 0) {
        fmt::print(stderr, "Parsing failed\n");
        exit(EXIT_FAILURE);
    }

    transUnit = std::shared_ptr<AST_TranslationUnit>(res);
    res = nullptr;
}

std::shared_ptr<AST_TranslationUnit> CoreDriver::getTransUnit() {
    return transUnit;
}

CoreParserState *CoreDriver::getPState() {
    return pstate.get();
}

void CoreDriver::lexer_error(const char *msg) {
    auto fixLoc = warps.getLoc(location.begin.line);
    std::string filename = warps.getFilename(fixLoc.filenum);
    fmt::print(stderr, "Lexer error ({}:{}:{}):\n\t{}\n",
               filename, fixLoc.line, location.begin.column, msg);
}


void yy::parser::error(const location_type &loc, const std::string &msg) {
    auto fixLoc = drv.warps.getLoc(loc.begin.line);
    std::string filename = drv.warps.getFilename(fixLoc.filenum);
    fmt::print(stderr, "Parsing error ({}:{}:{}):\n\t{}\n",
               filename, fixLoc.line, loc.begin.column, msg);

//    std::cerr << loc << ": " << msg << '\n';
//    exit(EXIT_FAILURE);
}
