#include "preprocessor.hpp"
#include <fstream>
#include <sstream>
#include <stack>
#include <ctime>
#include <fmt/ostream.h>
#include <fmt/core.h>
#include <fmt/chrono.h>

class PreprocessorImpl {
public:
    PreprocessorImpl(std::string const &path, std::map<std::string, std::string> &defines);

    std::string finalText() const;
    LinesWarpMap&& moveWarps();

private:
    std::map<std::string, std::string> &defines;
    LinesWarpMap warps;

    size_t globalLine = 0;

    std::stack<std::string> raw;
    std::stringstream globalSS;

    bool isSkip;
    int nestCntr;

    enum LastCondState {
        PC_IF_TRUE, PC_IF_FALSE, PC_ELSE
    };
    std::stack<LastCondState> condStatuses;

    struct Location {
        std::string file;
        int line;
    };
    std::stack<Location> locations;

    std::time_t trTime;

    using string_constit_t = decltype(raw.top().cbegin());

    void processFile(std::string const &path);
    void processLine(string_constit_t &it);
    void processDirective(std::string const &dir, string_constit_t &it);
    void processComment(string_constit_t &it);

    std::string getStringArg(string_constit_t &it, bool angleBrackets);
    uint32_t getLineNumArg(string_constit_t &it);
    std::string getIdentArg(string_constit_t &it);

    void skipSpaces(string_constit_t &it);
    void assertNoArg(string_constit_t &it);
    std::string scanIdent(string_constit_t &it);
    void passNumber(string_constit_t &it);
    void putIdent(std::string const &ident);

    bool noEnd(string_constit_t const &it) const;
    [[nodiscard]] std::string readFile(std::string const &path);

    [[noreturn]] void printError(std::string const &msg);
    void printWarn(std::string const &msg);
};


PreprocessorImpl::PreprocessorImpl(const std::string &path,
                                   std::map<std::string, std::string> &defines)
        : defines(defines), warps(path) {
    trTime = time(nullptr);
    processFile(path);
}

std::string PreprocessorImpl::finalText() const {
    return globalSS.str();
}

LinesWarpMap&& PreprocessorImpl::moveWarps() {
    return std::move(warps);
}


void PreprocessorImpl::printError(std::string const &msg) {
    if (!locations.empty()) {
        auto const &loc = locations.top();
        fmt::print(stderr, "Error ({}:{}): {}\n", loc.file, loc.line, msg);
    }
    else {
        fmt::print(stderr, "Error: {}\n", msg);
    }
    exit(EXIT_FAILURE);
}

void PreprocessorImpl::printWarn(std::string const &msg) {
    auto const &loc = locations.top();
    fmt::print(stderr, "Warning ({}:{}): {}\n", loc.file, loc.line, msg);
}


std::string PreprocessorImpl::readFile(std::string const &path) {
    std::ifstream ifs(path);
    if (!ifs.is_open())
        printError(fmt::format("Error: Cannot read file '{}'\n", path));
    std::stringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}


bool PreprocessorImpl::noEnd(string_constit_t const &it) const {
    return it != raw.top().cend();
}


void PreprocessorImpl::processFile(const std::string &path) {
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

void PreprocessorImpl::processLine(string_constit_t &it) {
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

        if (!std::isalpha(*it))
            printError("Expected directive name");

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

                    if (!noEnd(it))
                        printError("Incomplete escape sequence");

                    if (*it != '\n') { // Lines concatenator
                        globalSS.put('\\');
                        globalSS.put(*(it++));
                    }
                }
                else {
                    globalSS.put(*(it++));
                }
            }
            if (!noEnd(it) || *it != '"')
                printError("Incomplete string");

            it++;
            globalSS.put('"');
        }
        else if (*it == '\'') {
            it++;
            globalSS.put('\'');
            if (noEnd(it) && *it == '\\') {
                it++;

                if (!noEnd(it))
                    printError("Incomplete escape sequence");

                globalSS.put('\\');
                globalSS.put(*(it++));
            }
            else if (noEnd(it)) {
                globalSS.put(*(it++));
            }

            if (!noEnd(it) || *it != '\'')
                printError("Incomplete character literal");

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

std::string PreprocessorImpl::scanIdent(string_constit_t &it) {
    std::stringstream ss;
    // Assume that first character is alphabetic or '_'
    while (noEnd(it) && (isalnum(*it) || *it == '_'))
        ss << *(it++);
    return ss.str();
}

void PreprocessorImpl::passNumber(string_constit_t &it) {
    // Assume that current character is digit
    while (noEnd(it) && isalnum(*it)) // Doesn't care about number correctess
        globalSS.put(*(it++));
}

void PreprocessorImpl::processComment(string_constit_t &it) {
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

void PreprocessorImpl::processDirective(std::string const &dir, string_constit_t &it) {
    if (dir == "include") {
        if (isSkip)
            return;
        std::string filename = getStringArg(it, *it == '<');
        assertNoArg(it);

        if (raw.size() > Preprocessor::MAX_INCLUDES_DEPTH) {
            printError(fmt::format(
                    "Too many recursive includes (depth {})",
                    Preprocessor::MAX_INCLUDES_DEPTH));
        }

        warps.appendWarpLoc(globalLine, 0, filename);
        processFile(filename);
        warps.appendWarpLoc(globalLine, locations.top().line - 1, locations.top().file);
    }
    else if (dir == "define") {
        if (isSkip)
            return;
        std::string name = getIdentArg(it);

        if (name.empty())
            printError("Directive #define expects argument");

        assertNoArg(it);
        defines.emplace(std::move(name), ""); // TODO: macros
    }
    else if (dir == "undef") {
        std::string name = getIdentArg(it);
        if (isSkip)
            return;

        if (name.empty())
            printError("Directive #undef expects argument");

        assertNoArg(it);
        defines.erase(name);
    }
    else if (dir == "ifdef" || dir == "ifndef") {
        if (isSkip) {
            nestCntr++;
            return;
        }
        std::string arg = getIdentArg(it);

        if (arg.empty())
            printError(fmt::format("Directive #{} expects argument", dir));

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

        if (condStatuses.top() == PC_IF_FALSE)
            isSkip = false;
        else if (condStatuses.top() == PC_IF_TRUE)
            isSkip = true;
        else
            printError("Unexpected #else directive");

        condStatuses.pop();
        condStatuses.push(PC_ELSE);
    }
    else if (dir == "line") {
        if (isSkip)
            return;
        uint32_t arg = getLineNumArg(it);
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
        printError(fmt::format("#error \"{}\"", msg)); // Exit here
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
        printError(fmt::format("Unknown preprocessor directive ({})", dir)); // Exit here
    }
}

void PreprocessorImpl::putIdent(const std::string &ident) {
    if (ident.starts_with("__")) { // Small optimization for predefined macros
        if (ident == "__FILE__") {
            std::string const &filename = locations.top().file;
            fmt::print(globalSS, "\"{:s}\"", filename);
            return;
        }
        else if (ident == "__LINE__") {
            globalSS << locations.top().line;
            return;
        }
        else if (ident == "__DATE__") { // "Mar 27 2022", "Jan  1 2022"
            fmt::print(globalSS, "\"{:%b %e %Y}\"", fmt::localtime(trTime));
            return;
        }
        else if (ident == "__TIME__") { // "15:24:12"
            fmt::print(globalSS, "\"{:%H:%M:%S}\"", fmt::localtime(trTime));
            return;
        }
    }

    globalSS.write(ident.c_str(), ident.size());
}


std::string PreprocessorImpl::getStringArg(string_constit_t &it, bool angleBrackets) {
    if ((angleBrackets && *it != '<') || (!angleBrackets && *it != '"'))
        printError(fmt::format("Wrong quote symbol ({})", *it));
    ++it;

    std::stringstream ss;
    char qclose = angleBrackets ? '>' : '"';
    while (noEnd(it) && *it != qclose)
        ss << *(it++);

    if (!noEnd(it) || *it != qclose)
        printError(fmt::format("Expected closing quote ('{}')", qclose));
    ++it;

    return ss.str();
}

uint32_t PreprocessorImpl::getLineNumArg(string_constit_t &it) {
    std::stringstream ss;
    while (noEnd(it) && std::isdigit(*it))
        ss << *(it++);

    std::string snum = ss.str();
    char *endptr;
    uint64_t val = std::strtoul(snum.c_str(), &endptr, 10);

    if (*endptr != '\0' || val > INT32_MAX)
        printError(fmt::format("Wrong line number format: {}", snum));

    return static_cast<uint32_t>(val);
}

std::string PreprocessorImpl::getIdentArg(string_constit_t &it) {
    if (!noEnd(it) || !(std::isalpha(*it) || *it == '_'))
        printError(fmt::format("Wrong identifier format"));

    std::stringstream ss;
    ss << *(it++);
    while (noEnd(it) && (std::isalnum(*it) || *it == '_'))
        ss << *(it++);
    return ss.str();
}

void PreprocessorImpl::skipSpaces(string_constit_t &it) {
    while (noEnd(it) && std::isspace(*it) && *it != '\n')
        ++it;
}

void PreprocessorImpl::assertNoArg(string_constit_t &it) {
    skipSpaces(it);
    if (noEnd(it) && *it != '\n')
        printError("Too many arguments for directive");
}


Preprocessor::Preprocessor(std::string const &path) {
    auto impl = PreprocessorImpl(path, defines);
    finalText = impl.finalText();
    warps = std::make_unique<LinesWarpMap>(impl.moveWarps());
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
    return *warps;
}
