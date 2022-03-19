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

Preprocessor::Preprocessor(const std::string &path) {
    raw = readFile(path);
}

void Preprocessor::addDefine(std::string name, std::string value) {
    defines.emplace(std::move(name), std::move(value));
}

void Preprocessor::removeDefine(std::string const &name) {
    defines.erase(name);
}

void Preprocessor::printError(std::string const &msg) {
    fmt::print(stderr, "Error in line {}: {}\n", curLine, msg);
}

void Preprocessor::printWarn(std::string const &msg) {
    fmt::print(stderr, "Warning in line {}: {}\n", curLine, msg);
}

std::string Preprocessor::process() {
    std::stringstream ss;

    isLineStart = true;
    isSkip = false;
    nestCntr = 0;
    cond_statuses = std::stack<LastCondState>();

    curLine = 1;

    auto it = raw.cbegin();
    while (it != raw.cend()) {
        char ch = *(it++);
        if (ch == '\n') {
            isLineStart = true;
            ss << ch;
            curLine++;
        }
        else if (isLineStart && ch == '#') {
            skip_spaces(it);
            if (!is_alphachar(*it)) {
                printError("Expected directive name");
                exit(EXIT_FAILURE);
            }
            std::string directive = get_ident(it);
            process_directive(directive, it);
        }
        else {
            if (!isSkip)
                ss << ch;
            if (isLineStart && !std::isspace(ch))
                isLineStart = false;
        }
    }

    return ss.str();
}

void Preprocessor::process_directive(std::string const &dir, string_constit_t &it) {
    if (dir == "define") {
        std::string name = get_directive_arg(it);
        if (isSkip)
            return;
        if (name.empty()) {
            printError("Directive expects argument");
            exit(EXIT_FAILURE);
        }
        assert_no_directive_arg(it);
        addDefine(name, ""); // TODO
    }
    else if (dir == "undef") {
        std::string name = get_directive_arg(it);
        if (isSkip)
            return;
        if (name.empty()) {
            printError("Directive expects argument");
            exit(EXIT_FAILURE);
        }
        assert_no_directive_arg(it);
        removeDefine(name);
    }
    else if (dir == "ifdef") {
        std::string arg = get_directive_arg(it);
        if (isSkip) {
            nestCntr++;
            return;
        }
        if (arg.empty()) {
            printError("Directive expects argument");
            exit(EXIT_FAILURE);
        }
        assert_no_directive_arg(it);
        if (defines.contains(arg)) {
            cond_statuses.push(PC_IF_TRUE);
        }
        else {
            cond_statuses.push(PC_IF_FALSE);
            isSkip = true;
        }
    }
    else if (dir == "endif") {
        if (isSkip && nestCntr > 0) {
            nestCntr--;
            return;
        }
        assert_no_directive_arg(it);
        cond_statuses.pop();
        isSkip = false;
    }
    else if (dir == "else") {
        if (isSkip && nestCntr > 0)
            return;
        assert_no_directive_arg(it);
        if (cond_statuses.top() == PC_IF_FALSE) {
            isSkip = false;
        }
        else if (cond_statuses.top() == PC_IF_TRUE) {
            isSkip = true;
        }
        else {
            printError("Unexpected #else directive");
            exit(EXIT_FAILURE);
        }
        cond_statuses.pop();
        cond_statuses.push(PC_ELSE);
    }
    else if (dir == "line") {
        std::string arg = get_directive_arg(it);
        if (isSkip)
            return;
        assert_no_directive_arg(it);

        char *endptr;
        uint64_t line = std::strtoul(arg.c_str(), &endptr, 10);
        if (*endptr != '\0' || line > INT32_MAX) {
            printError(fmt::format("Wrong line number format: {}", arg));
            exit(EXIT_FAILURE);
        }
        curLine = line;
    }
    else if (dir == "error") {
        std::stringstream ss;
        while (it != raw.cend() && *it != '\n')
            ss << *(it++);
        if (isSkip)
            return;
        printError(fmt::format("#error {}", ss.str()));
        exit(EXIT_FAILURE);
    }
    else if (dir == "warning") {
        std::stringstream ss;
        while (it != raw.cend() && *it != '\n')
            ss << *(it++);
        if (isSkip)
            return;
        printWarn(fmt::format("#warning {}", ss.str()));
    }
    else {
        printError(fmt::format("Unknown preprocessor directive ({})", dir));
        exit(EXIT_FAILURE);
    }
}

std::string Preprocessor::get_directive_arg(Preprocessor::string_constit_t &it) {
    skip_spaces(it);
    if (it == raw.cend() || *it == '\n')
        return "";
    std::string res = get_ident(it);
    skip_spaces(it);
    return res;
}

void Preprocessor::assert_no_directive_arg(Preprocessor::string_constit_t &it) {
    skip_spaces(it);
    if (it != raw.cend() && *it != '\n') {
        printError("Too many arguments for directive");
        exit(EXIT_FAILURE);
    }
}

std::string Preprocessor::get_ident(Preprocessor::string_constit_t &it) {
//    if (it == raw.cend() || !is_alphachar(*it))
//        throw;
    std::stringstream ss;
    ss << *(it++);
    while (it != raw.cend() && is_alphanum(*it))
        ss << *(it++);
    return ss.str();
}

void Preprocessor::skip_spaces(string_constit_t &it) {
    while (it != raw.cend() && std::isspace(*it) && *it != '\n')
        it++;
}
