#include "preprocessor.hpp"
#include <fstream>
#include <sstream>
#include <fmt/ostream.h>
#include <fmt/core.h>
#include <fmt/chrono.h>

static std::string readFile(std::string const &path) {
    std::ifstream t(path.c_str());
    if (!t.is_open()) {
        fmt::print(stderr, "Cannot read file '{}'\n", path);
        exit(EXIT_FAILURE);
    }
    t.seekg(0, std::ios::end);
    auto size = static_cast<long>(t.tellg());
    std::string buffer(size, ' '); // TODO: don't fill buffer
    t.seekg(0);
    t.read(&buffer[0], size);
    return buffer;
}


Preprocessor::Preprocessor(std::string const &path) : warps(path) {
    globalLine = 0;
    trTime = time(nullptr);

    processFile(path);
    finalText = globalSS.str();
}

void Preprocessor::addDefine(std::string name, std::string value) {
    defines.emplace(std::move(name), std::move(value));
}

void Preprocessor::removeDefine(std::string const &name) {
    defines.erase(name);
}

std::string Preprocessor::getText() const {
    return finalText;
}

LinesWarpMap Preprocessor::getWarps() const {
    return warps;
}


void Preprocessor::printError(std::string const &msg) {
    auto const &loc = locations.top();
    fmt::print(stderr, "Error ({}:{}): {}\n", loc.file, loc.line, msg);
}

void Preprocessor::printWarn(std::string const &msg) {
    auto const &loc = locations.top();
    fmt::print(stderr, "Warning ({}:{}): {}\n", loc.file, loc.line, msg);
}


bool Preprocessor::noEnd(string_constit_t const &it) const {
    return it != raw.top().cend();
}


void Preprocessor::processFile(const std::string &path) {
    raw.push(readFile(path));
    locations.push({ path, 1 });

    isSkip = false;
    nestCntr = 0;
    condStatuses = std::stack<LastCondState>();

    auto it = raw.top().cbegin();
    while (noEnd(it)) {
        globalSS.put('\n');
        globalLine++;
        processLine(it);
        locations.top().line++;
        if (noEnd(it))
            it++;
    }

    raw.pop();
    locations.pop();
}

void Preprocessor::processLine(string_constit_t &it) {
    while (noEnd(it) && isspace(*it) && *it != '\n') {
        if (!isSkip)
            globalSS.put(' ');
        it++;
    }

    if (!noEnd(it))
        return;

    if (*it == '#') {
        it++;
        skipSpaces(it);
        if (!noEnd(it)) // Empty directive
            return;
        if (!std::isalpha(*it)) {
            printError("Expected directive name");
            exit(EXIT_FAILURE);
        }
        std::string directive = getIdentArg(it);
        skipSpaces(it);
        processDirective(directive, it);
        if (isSkip) {
            while (noEnd(it) && *it != '\n')
                it++;
        }
        return;
    }

    if (isSkip) {
        while (noEnd(it) && *it != '\n')
            it++;
        return;
    }

    while (noEnd(it) && *it != '\n') {
        if (isalpha(*it) || *it == '_') {
            std::string ident = scanIdent(it);
            putIdent(ident);
        }
        else if (isdigit(*it)) {
            passNumber(it);
        }
        else if (*it == '"') {
            it++;
            globalSS.put('"');
            while (noEnd(it) && *it != '"' && *it != '\n') {
                if (*it == '\\') {
                    it++;
                    if (!noEnd(it)) {
                        printError("Incomplete escape sequence");
                        exit(EXIT_FAILURE);
                    }
                    if (*it != '\n') { // Lines concatenator
                        globalSS.put('\\');
                        globalSS.put(*(it++));
                    }
                }
                else {
                    globalSS.put(*(it++));
                }
            }
            if (!noEnd(it) || *it != '"') {
                printError("Incomplete string");
                exit(EXIT_FAILURE);
            }
            it++;
            globalSS.put('"');
        }
        else if (*it == '\'') {
            it++;
            globalSS.put('\'');
            if (noEnd(it) && *it == '\\') {
                it++;
                if (!noEnd(it)) {
                    printError("Incomplete escape sequence");
                    exit(EXIT_FAILURE);
                }
                globalSS.put('\\');
                globalSS.put(*(it++));
            }
            else if (noEnd(it)) {
                globalSS.put(*(it++));
            }
            if (!noEnd(it) || *it != '\'') {
                printError("Incomplete character literal");
                exit(EXIT_FAILURE);
            }
            it++;
            globalSS.put('\'');
        }
        else if (*it == '/') {
            if (noEnd(it + 1)) {
                if (*(it + 1) == '/') {
                    it += 2;
                    while (noEnd(it) && *it != '\n')
                        it++;
                    return;
                }
                else if (*(it + 1) == '*') {
                    it += 2;
                    globalSS.write("  ", 2);
                    processComment(it);
                    continue;
                }
            }
            else {
                globalSS.put('/');
            }
        }
        else {
            globalSS.put(*(it++));
        }
    }
}

std::string Preprocessor::scanIdent(string_constit_t &it) {
    std::stringstream ss;
    // Assume that first character is alphabetic or '_'
    while (noEnd(it) && (isalnum(*it) || *it == '_'))
        ss << *(it++);
    return ss.str();
}

void Preprocessor::passNumber(string_constit_t &it) {
    // Assume that current character is digit
    while (noEnd(it) && isalnum(*it)) // Doesn't care about number correctess
        globalSS.put(*(it++));
}

void Preprocessor::processComment(string_constit_t &it) {
    auto startLoc = locations.top();
    while (true) {
        while (noEnd(it) && *it != '*') {
            if (*it == '\n') {
                globalSS.put('\n');
                locations.top().line++;
                globalLine++;
            }
            else {
                globalSS.put(' ');
            }
            it++;
        }
        if (!noEnd(it)) {
            locations.top() = startLoc;
            printError("Unterminated comment");
            exit(EXIT_FAILURE);
        }
        it++;
        globalSS.put(' ');

        if (noEnd(it) && *it == '/') {
            it++;
            globalSS.put(' ');
            break;
        }
    }
}

void Preprocessor::processDirective(std::string const &dir, string_constit_t &it) {
    if (dir == "include") {
        if (isSkip)
            return;
        std::string filename = getStringArg(it, *it == '<');
        assertNoArg(it);

        if (raw.size() > MAX_INCLUDES_DEPTH) {
            printError(fmt::format("Too many recursive includes (depth {})",
                                   MAX_INCLUDES_DEPTH));
            exit(EXIT_FAILURE);
        }

        warps.appendWarpLoc(globalLine, 0, filename);
        processFile(filename);
        warps.appendWarpLoc(globalLine, locations.top().line - 1, locations.top().file);
    }
    else if (dir == "define") {
        if (isSkip)
            return;
        std::string name = getIdentArg(it);
        if (name.empty()) {
            printError("Directive expects argument");
            exit(EXIT_FAILURE);
        }
        assertNoArg(it);
        addDefine(name, ""); // TODO
    }
    else if (dir == "undef") {
        std::string name = getIdentArg(it);
        if (isSkip)
            return;
        if (name.empty()) {
            printError("Directive expects argument");
            exit(EXIT_FAILURE);
        }
        assertNoArg(it);
        removeDefine(name);
    }
    else if (dir == "ifdef" || dir == "ifndef") {
        if (isSkip) {
            nestCntr++;
            return;
        }
        std::string arg = getIdentArg(it);
        if (arg.empty()) {
            printError("Directive expects argument");
            exit(EXIT_FAILURE);
        }
        assertNoArg(it);

        if (defines.contains(arg) || dir == "ifndef") {
            condStatuses.push(PC_IF_TRUE);
        }
        else {
            condStatuses.push(PC_IF_FALSE);
            isSkip = true;
        }
    }
    else if (dir == "endif") {
        if (isSkip && nestCntr > 0) {
            nestCntr--;
            return;
        }
        assertNoArg(it);
        condStatuses.pop();
        isSkip = false;
    }
    else if (dir == "else") {
        if (isSkip && nestCntr > 0)
            return;
        assertNoArg(it);
        if (condStatuses.top() == PC_IF_FALSE) {
            isSkip = false;
        }
        else if (condStatuses.top() == PC_IF_TRUE) {
            isSkip = true;
        }
        else {
            printError("Unexpected #else directive");
            exit(EXIT_FAILURE);
        }
        condStatuses.pop();
        condStatuses.push(PC_ELSE);
    }
    else if (dir == "line") {
        if (isSkip)
            return;
        uint32_t arg = getU32Arg(it);
        skipSpaces(it);
        std::string filename;
        if (noEnd(it) && *it == '"')
            filename = getStringArg(it, false);
        assertNoArg(it);

        locations.top().line = arg;
        if (!filename.empty())
            locations.top().file = filename;
        warps.appendWarpLoc(globalLine, locations.top().line, locations.top().file);
    }
    else if (dir == "error") {
        if (isSkip)
            return;
        std::string msg = getStringArg(it, false);
        assertNoArg(it);
        printError(fmt::format("#error \"{}\"", msg));
        exit(EXIT_FAILURE);
    }
    else if (dir == "warning") {
        if (isSkip)
            return;
        std::string msg = getStringArg(it, false);
        assertNoArg(it);
        printWarn(fmt::format("#warning \"{}\"", msg));
    }
    else {
        if (isSkip)
            return;
        printError(fmt::format("Unknown preprocessor directive ({})", dir));
        exit(EXIT_FAILURE);
    }
}

void Preprocessor::putIdent(const std::string &ident) {
    if (ident == "__FILE__") {
        std::string const &filename = locations.top().file;
        globalSS.put('"');
        globalSS.write(filename.c_str(), filename.size());
        globalSS.put('"');
    }
    else if (ident == "__LINE__") {
        globalSS << locations.top().line;
    }
    else if (ident == "__DATE__") { // Mar 27 2022, Jan  1 2022
        fmt::print(globalSS, "\"{:%b %e %Y}\"", fmt::localtime(trTime));
    }
    else if (ident == "__TIME__") {
        fmt::print(globalSS, "\"{:%H:%M:%S}\"", fmt::localtime(trTime));
    }
    else {
        globalSS.write(ident.c_str(), ident.size());
    }
}


std::string Preprocessor::getStringArg(string_constit_t &it, bool angleBrackets) {
    if ((angleBrackets && *it != '<') || (!angleBrackets && *it != '"')) {
        printError(fmt::format("Wrong quote symbol ({})", *it));
        exit(EXIT_FAILURE);
    }
    ++it;

    std::stringstream ss;
    char qclose = angleBrackets ? '>' : '"';
    while (noEnd(it) && *it != qclose)
        ss << *(it++);

    if (!noEnd(it) || *it != qclose) {
        printError(fmt::format("Expected closing quote ('{}')", qclose));
        exit(EXIT_FAILURE);
    }
    ++it;

    return ss.str();
}

uint32_t Preprocessor::getU32Arg(Preprocessor::string_constit_t &it) {
    std::stringstream ss;
    while (noEnd(it) && std::isdigit(*it))
        ss << *(it++);

    std::string snum = ss.str();
    char *endptr;
    uint64_t val = std::strtoul(snum.c_str(), &endptr, 10);
    if (*endptr != '\0' || val > INT32_MAX) {
        printError(fmt::format("Wrong line number format: {}", snum));
        exit(EXIT_FAILURE);
    }
    return static_cast<uint32_t>(val);
}

std::string Preprocessor::getIdentArg(string_constit_t &it) {
    if (!noEnd(it) || !(std::isalpha(*it) || *it == '_')) {
        printError(fmt::format("Wrong identifier format"));
        exit(EXIT_FAILURE);
    }
    std::stringstream ss;
    ss << *(it++);
    while (noEnd(it) && (std::isalnum(*it) || *it == '_'))
        ss << *(it++);
    return ss.str();
}

void Preprocessor::skipSpaces(string_constit_t &it) {
    while (noEnd(it) && std::isspace(*it) && *it != '\n')
        ++it;
}

void Preprocessor::assertNoArg(string_constit_t &it) {
    skipSpaces(it);
    if (noEnd(it) && *it != '\n') {
        printError("Too many arguments for directive");
        exit(EXIT_FAILURE);
    }
}
