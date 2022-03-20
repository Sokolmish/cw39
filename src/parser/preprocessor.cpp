#include "preprocessor.hpp"
#include <fstream>
#include <sstream>
#include <fmt/core.h>

static inline bool is_alphachar(char ch) {
    return std::isalpha(ch) || ch == '_';
}

static inline bool is_alphanum(char ch) {
    return std::isalnum(ch) || ch == '_';
}

static std::string readFile(std::string const &path) {
    std::ifstream t(path.c_str());
    if (!t.is_open()) {
        fmt::print(stderr, "Cannot read file '{}'\n", path);
        exit(EXIT_FAILURE);
    }
    t.seekg(0, std::ios::end);
    auto size = t.tellg();
    std::string buffer(size, ' ');
    t.seekg(0);
    t.read(&buffer[0], size);
    return buffer;
}


Preprocessor::Preprocessor() = default;

void Preprocessor::addDefine(std::string name, std::string value) {
    defines.emplace(std::move(name), std::move(value));
}

void Preprocessor::removeDefine(std::string const &name) {
    defines.erase(name);
}

std::string Preprocessor::process(std::string const &path) {
    processFile(path);
    return globalSS.str();
}


void Preprocessor::printError(std::string const &msg) {
    auto const &loc = locations.top();
    fmt::print(stderr, "Error ({}:{}): {}\n", loc.file, loc.line, msg);
}

void Preprocessor::printWarn(std::string const &msg) {
    auto const &loc = locations.top();
    fmt::print(stderr, "Warning ({}:{}): {}\n", loc.file, loc.line, msg);
}


void Preprocessor::processFile(const std::string &path) {
    raw.push(readFile(path));
    locations.push({ path, 1 });

    isLineStart = true;
    isSkip = false;
    nestCntr = 0;
    condStatuses = std::stack<LastCondState>();

    auto it = raw.top().cbegin();
    while (it != raw.top().cend()) {
        char ch = *(it++);
        if (ch == '\n') {
            isLineStart = true;
            globalSS << ch;
            locations.top().line++;
        }
        else if (isLineStart && ch == '#') {
            skipSpaces(it);
            if (!is_alphachar(*it)) {
                printError("Expected directive name");
                exit(EXIT_FAILURE);
            }
            std::string directive = getIdentArg(it);
            skipSpaces(it);
            processDirective(directive, it);
        }
        else {
            if (!isSkip)
                globalSS << ch;
            if (isLineStart && !std::isspace(ch))
                isLineStart = false;
        }
    }

    raw.pop();
    locations.pop();
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
        processFile(filename);
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
        if (it != raw.top().cend() && *it == '"')
            filename = getStringArg(it, false);
        assertNoArg(it);
        static_cast<void>(filename);
        locations.top().line = arg;
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

std::string Preprocessor::getStringArg(string_constit_t &it, bool angleBrackets) {
    if ((angleBrackets && *it != '<') || (!angleBrackets && *it != '"')) {
        printError(fmt::format("Wrong quote symbol ({})", *it));
        exit(EXIT_FAILURE);
    }
    it++;

    std::stringstream ss;
    char qclose = angleBrackets ? '>' : '"';
    while (it != raw.top().cend() && *it != qclose)
        ss << *(it++);

    if (it == raw.top().cend() || *it != qclose) {
        printError(fmt::format("Expected closing quote ('{}')", qclose));
        exit(EXIT_FAILURE);
    }
    it++;

    return ss.str();
}

uint32_t Preprocessor::getU32Arg(Preprocessor::string_constit_t &it) {
    std::stringstream ss;
    while (it != raw.top().cend() && std::isdigit(*it))
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
    if (it == raw.top().cend() || !is_alphachar(*it)) {
        printError(fmt::format("Wrong identifier format"));
        exit(EXIT_FAILURE);
    }
    std::stringstream ss;
    ss << *(it++);
    while (it != raw.top().cend() && is_alphanum(*it))
        ss << *(it++);
    return ss.str();
}

void Preprocessor::skipSpaces(string_constit_t &it) {
    while (it != raw.top().cend() && std::isspace(*it) && *it != '\n')
        it++;
}

void Preprocessor::assertNoArg(string_constit_t &it) {
    skipSpaces(it);
    if (it != raw.top().cend() && *it != '\n') {
        printError("Too many arguments for directive");
        exit(EXIT_FAILURE);
    }
}
