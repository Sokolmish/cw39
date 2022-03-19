#include "preprocessor.hpp"
#include <fstream>
#include <sstream>
#include <fmt/core.h>


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

std::string Preprocessor::process() {
    std::stringstream ss;

    isLineStart = true;
    isSkip = false;
    nestCntr = 0;
    cond_statuses = std::stack<LastCondState>();

    for (auto it = raw.cbegin(); it != raw.cend(); it++) {
        char ch = *it;

        if (ch == '\n') {
            isLineStart = true;
            ss << ch;
            continue;
        }

        if (isLineStart && ch == '#') {
            it++;
            std::string directive = std::isalpha(*it) ? get_ident(it) : "WRONG_DIRECTIVE";
            process_directive(directive, it);
            continue;
        }

        if (!isSkip) {
            ss << ch;
        }
        if (isLineStart && !std::isspace(ch)) {
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
        addDefine(name, ""); // TODO
        assert_no_directive_arg(it);
    }
    else if (dir == "undef") {
        std::string name = get_directive_arg(it);
        if (isSkip)
            return;
        removeDefine(name);
        assert_no_directive_arg(it);
    }
    else if (dir == "ifdef") {
        std::string arg = get_directive_arg(it);
        if (isSkip) {
            nestCntr++;
            return;
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
        if (isSkip && nestCntr > 0) {
            return;
        }

        assert_no_directive_arg(it);

        if (cond_statuses.top() == PC_IF_FALSE) {
            isSkip = false;
        }
        else if (cond_statuses.top() == PC_IF_TRUE) {
            isSkip = true;
        }
        else {
            fmt::print(stderr, "Unexpected #else directive\n");
            exit(EXIT_FAILURE);
        }
        cond_statuses.pop();
        cond_statuses.push(PC_ELSE);
    }
    else {
        fmt::print(stderr, "Unknown preprocessor directive: '{}'\n", dir);
        exit(EXIT_FAILURE);
    }
}

std::string Preprocessor::get_directive_arg(Preprocessor::string_constit_t &it) {
    skip_spaces(it);
    std::string res = get_ident(it);
    skip_spaces(it);
    return res;
}

void Preprocessor::assert_no_directive_arg(Preprocessor::string_constit_t &it) {
    skip_spaces(it);
    if (it != raw.cend() && *it != '\n') {
        fmt::print(stderr, "Too many arguments for directive\n");
        exit(EXIT_FAILURE);
    }
}

std::string Preprocessor::get_ident(Preprocessor::string_constit_t &it) {
//    if (it == raw.cend() || !std::isalpha(*it))
//        throw;

    std::stringstream ss;
    ss << *it;
    it++;
    while (it != raw.cend() && std::isalnum(*it)) {
        ss << *it;
        it++;
    }
    return ss.str();
}

void Preprocessor::skip_spaces(string_constit_t &it) {
    while (it != raw.cend() && std::isspace(*it) && *it != '\n') {
        it++;
    }
}
