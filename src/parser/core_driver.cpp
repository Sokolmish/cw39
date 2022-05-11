#include "core_driver.hpp"
#include <fmt/core.h>
#include <cctype>

CoreDriver::CoreDriver(ParsingContext &ctx, std::string program)
        : ctx(ctx), text(std::move(program)) {
    trace_parsing = false;
    trace_scanning = false;

    if (text.empty() || isStringWhitespace(text))
        throw cw39_error("Cannot compile empty or whitespace file");

    parse();
}

bool CoreDriver::isStringWhitespace(const std::string &str) {
    return std::all_of(str.begin(), str.end(), isspace);
}

void CoreDriver::parse() {
    location.initialize(nullptr);
    scan_begin();
    yy::parser parse(scanner, *this);
    parse.set_debug_level(trace_parsing);
    int rc = parse();
    scan_end();

    if (rc != 0)
        throw parser_exception(fmt::format("Parsing failed with return code {}", rc), "");

    transUnit = std::shared_ptr<AST_TranslationUnit>(res);
    res = nullptr;
}

std::shared_ptr<AST_TranslationUnit> CoreDriver::getTransUnit() {
    return transUnit;
}

void CoreDriver::lexer_error(const char *msg) {
    auto fixLoc = ctx.warps.getLoc(location.begin.line);
    std::string filename = ctx.warps.getFilename(fixLoc.filenum);
    std::string excMsg = fmt::format("Lexer error: {}", msg);
    std::string excLoc = fmt::format("{}:{}:{}", filename, fixLoc.line, location.begin.column);
    // TODO: specialized exception or at least exc location
    throw parser_exception(std::move(excMsg), std::move(excLoc));
}


void yy::parser::error(const location_type &loc, const std::string &msg) {
    auto fixLoc = drv.ctx.warps.getLoc(loc.begin.line);
    std::string filename = drv.ctx.warps.getFilename(fixLoc.filenum);
    fmt::print(stderr, "Parsing error ({}:{}:{}):\n\t{}\n",
               filename, fixLoc.line, loc.begin.column, msg);

//    std::cerr << loc << ": " << msg << '\n';
//    exit(EXIT_FAILURE);
}
