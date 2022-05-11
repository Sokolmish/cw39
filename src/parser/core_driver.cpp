#include "core_driver.hpp"
#include <fmt/core.h>
#include <cctype>

static bool isStringWhitespace(const std::string &str) {
    return std::all_of(str.begin(), str.end(), isspace);
}

CoreDriver::CoreDriver(ParsingContext &ctx, std::string program, int flags)
        : ctx(ctx), ast(std::make_shared<AbstractSyntaxTree>()), text(std::move(program)) {
    trace_parsing = (flags & TRACE_PARSER) != 0;
    trace_scanning = (flags & TRACE_SCANNER) != 0;

    if (text.empty() || isStringWhitespace(text))
        throw cw39_error("Cannot compile empty or whitespace file");

    location.initialize(nullptr);
    scan_begin();
    yy::parser parse(scanner, *this);
    parse.set_debug_level(trace_parsing);
    int rc = parse();
    scan_end();

    if (rc != 0)
        throw parser_exception(fmt::format("Parsing failed with return code {}", rc), "");
}

std::shared_ptr<AbstractSyntaxTree> CoreDriver::getAST() {
    return ast;
}

void CoreDriver::lexer_error(std::string msg) {
    auto fixLoc = ctx.warps.getLoc(location.begin.line);
    std::string filename = ctx.warps.getFilename(fixLoc.filenum);
    std::string excLoc = fmt::format("{}:{}:{}", filename, fixLoc.line, location.begin.column);
    std::string excMsg = fmt::format("Lexer error: {}", std::move(msg));
    throw parser_exception(std::move(excMsg), std::move(excLoc));
}


void yy::parser::error(const location_type &loc, const std::string &msg) {
    auto fixLoc = drv.ctx.warps.getLoc(loc.begin.line);
    std::string filename = drv.ctx.warps.getFilename(fixLoc.filenum);
    std::string excLoc = fmt::format("{}:{}:{}", filename, fixLoc.line, loc.begin.column);
    throw parser_exception(msg, std::move(excLoc));
}
