#include "preprocessor.hpp"
#include "cli_args.hpp" // For version
#include <fstream>
#include <sstream>
#include <stack>
#include <ctime>
#include <fmt/ostream.h>
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <utils.hpp>

#include "directives_hashtable.cpp"

class PreprocessorImpl {
public:
    PreprocessorImpl(Preprocessor &parent, const std::string &path);

private:
    Preprocessor &par;

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
    std::string_view baseFilename;
    unsigned long usrCounter;

    using string_constit_t = decltype(raw.top().cbegin());

    void processFile(std::string const &path);
    void processLine(string_constit_t &it);
    void processDirective(string_constit_t &it);

    void processSLComment(string_constit_t &it);
    void processMLComment(string_constit_t &it);

    void directiveInclude(string_constit_t &it);
    void directiveDefine(string_constit_t &it);
    void directiveUndef(string_constit_t &it);
    void directiveIfdef(string_constit_t &it, bool isNdef);
    void directiveElse(string_constit_t &it);
    void directiveEndif(string_constit_t &it);
    void directiveLine(string_constit_t &it);
    void directiveError(string_constit_t &it);
    void directiveWarning(string_constit_t &it);

    std::string getStringArg(string_constit_t &it, bool angleBrackets);
    uint32_t getLineNumArg(string_constit_t &it);
    std::string getIdentArg(string_constit_t &it);

    void skipSpaces(string_constit_t &it);
    void assertNoArg(string_constit_t &it);
    std::string scanIdent(string_constit_t &it);
    void putIdent(std::string const &ident);
    void passNumber(string_constit_t &it);
    void passString(string_constit_t &it);
    void passChar(string_constit_t &it);

    bool noEnd(string_constit_t const &it) const;
    [[nodiscard]] std::string readFile(std::string const &path);

    [[noreturn]] void printError(std::string const &msg);
    void printWarn(std::string const &msg);


    class pc_exception : public cw39_exception {
    public:
        pc_exception(std::string msg);
        pc_exception(Location const &loc, std::string msg);

    private:
        std::string formLoc(Location const &loc);
    };
};


PreprocessorImpl::PreprocessorImpl(Preprocessor &parent, std::string const &path) : par(parent) {
    trTime = time(nullptr);
    baseFilename = path;
    usrCounter = 0;
    processFile(path);
    par.finalText = globalSS.str();
}


PreprocessorImpl::pc_exception::pc_exception(std::string msg)
        : cw39_exception("Preprocessor error", "", std::move(msg)) {}

PreprocessorImpl::pc_exception::pc_exception(Location const &loc, std::string msg)
        : cw39_exception("Preprocessor error", formLoc(loc), std::move(msg)) {}

std::string PreprocessorImpl::pc_exception::formLoc(const PreprocessorImpl::Location &loc) {
    return fmt::format("{}:{}", loc.file, loc.line);
}


void PreprocessorImpl::printError(std::string const &msg) {
    if (!locations.empty())
        throw pc_exception(locations.top(), msg);
    else
        throw pc_exception(msg);
}

void PreprocessorImpl::printWarn(std::string const &msg) {
    auto const &loc = locations.top();
    fmt::print(stderr, "Warning ({}:{}): {}\n", loc.file, loc.line, msg);
}


std::string PreprocessorImpl::readFile(std::string const &path) {
    std::ifstream ifs(path);
    if (!ifs.is_open())
        printError(fmt::format("Cannot read file '{}'", path));
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
        globalLine++;
        processLine(it);
        globalSS.put('\n');
        locations.top().line++;
        if (noEnd(it))
            it++;
    }

    raw.pop();
    locations.pop();
}

void PreprocessorImpl::processLine(string_constit_t &it) {
    // Skip leading spaces
    while (noEnd(it) && isspace(*it) && *it != '\n') {
        if (!isSkip)
            globalSS.put(' ');
        it++;
    }

    // Empty line
    if (!noEnd(it))
        return;

    // Preprocessor directive
    if (*it == '#') {
        processDirective(it);
        return;
    }

    // Skip line under negative #ifdef
    if (isSkip) {
        while (noEnd(it) && *it != '\n')
            it++;
        return;
    }

    while (noEnd(it) && *it != '\n') {
        if (isalpha(*it) || *it == '_') { // Ident
            std::string ident = scanIdent(it);
            putIdent(ident);
        }
        else if (isdigit(*it)) { // Number
            passNumber(it);
        }
        else if (*it == '"') { // String
            passString(it);
        }
        else if (*it == '\'') { // Char
            passChar(it);
        }
        else if (*it == '/') { // Potential comment
            if (noEnd(it + 1)) {
                if (*(it + 1) == '/') { // Single line comment
                    processSLComment(it);
                    return;
                }
                else if (*(it + 1) == '*') { // Multiline comment
                    processMLComment(it);
                    continue;
                }
            }

            // There is no comment at this point
            ++it;
            globalSS.put('/');
        }
        else { // Other character
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

void PreprocessorImpl::passString(string_constit_t &it) {
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

void PreprocessorImpl::passChar(string_constit_t &it) {
    it++;
    globalSS.put('\'');
    if (noEnd(it) && *it == '\\') {
        it++;

        if (!noEnd(it))
            printError("Incomplete escape sequence");

        globalSS.put('\\');
        if (*it == 'x') {
            globalSS.put(*(it++)); // x
            globalSS.put(*(it++)); // H
            globalSS.put(*(it++)); // H
        }
        else
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

void PreprocessorImpl::processSLComment(string_constit_t &it) {
    it += 2;
    while (noEnd(it) && *it != '\n')
        it++;
}

void PreprocessorImpl::processMLComment(string_constit_t &it) {
    auto startLoc = locations.top();
    it += 2;
    globalSS.write("  ", 2);
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

void PreprocessorImpl::processDirective(string_constit_t &it) {
    it++;
    skipSpaces(it);
    if (!noEnd(it)) // Empty directive
        return;

    if (!std::isalpha(*it))
        printError("Expected directive name");

    std::string directive = getIdentArg(it);
    skipSpaces(it);

    auto dirEntry = PC_Directives::lookup(directive.c_str(), directive.size());
    if (dirEntry == nullptr) {
        if (isSkip)
            return;
        printError(fmt::format("Unknown preprocessor directive ({})", directive));
    }

    switch (dirEntry->id) {
        case PC_DIR_INCLUDE:
            directiveInclude(it);
            break;
        case PC_DIR_DEFINE:
            directiveDefine(it);
            break;
        case PC_DIR_UNDEF:
            directiveUndef(it);
            break;
        case PC_DIR_IFDEF:
            directiveIfdef(it, false);
            break;
        case PC_DIR_IFNDEF:
            directiveIfdef(it, true);
            break;
        case PC_DIR_ENDIF:
            directiveEndif(it);
            break;
        case PC_DIR_ELSE:
            directiveElse(it);
            break;
        case PC_DIR_LINE:
            directiveLine(it);
            break;
        case PC_DIR_ERROR:
            directiveError(it);
            break;
        case PC_DIR_WARNING:
            directiveWarning(it);
            break;
    }

    if (isSkip) {
        while (noEnd(it) && *it != '\n')
            it++;
    }
    return;
}


void PreprocessorImpl::directiveInclude(string_constit_t &it) {
    if (isSkip)
        return;
    std::string filename = getStringArg(it, *it == '<');
    assertNoArg(it);

    if (raw.size() > Preprocessor::MAX_INCLUDES_DEPTH) {
        printError(fmt::format(
                "Too many recursive includes (depth {})",
                Preprocessor::MAX_INCLUDES_DEPTH));
    }

    par.ctx->warps.appendWarpLoc(globalLine, 0, filename);
    processFile(filename);
    par.ctx->warps.appendWarpLoc(globalLine, locations.top().line - 1, locations.top().file);
}

void PreprocessorImpl::directiveDefine(string_constit_t &it) {
    if (isSkip)
        return;
    std::string name = getIdentArg(it);

    if (name.empty())
        printError("Directive #define expects argument");

    assertNoArg(it);
    par.defines.emplace(std::move(name), ""); // TODO: macros
}

void PreprocessorImpl::directiveUndef(string_constit_t &it) {
    std::string name = getIdentArg(it);
    if (isSkip)
        return;

    if (name.empty())
        printError("Directive #undef expects argument");

    assertNoArg(it);
    par.defines.erase(name);
}

void PreprocessorImpl::directiveIfdef(string_constit_t &it, bool isNdef) {
    if (isSkip) {
        ++nestCntr;
        return;
    }
    std::string arg = getIdentArg(it);

    if (arg.empty()) {
        std::string dirName = isNdef ? "ifndef" : "ifdef";
        printError(fmt::format("Directive #{} expects argument", dirName));
    }

    assertNoArg(it);

    if (par.defines.contains(arg) || isNdef) {
        condStatuses.push(PC_IF_TRUE);
    }
    else {
        condStatuses.push(PC_IF_FALSE);
        isSkip = true;
    }
}

void PreprocessorImpl::directiveElse(string_constit_t &it) {
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

void PreprocessorImpl::directiveEndif(string_constit_t &it) {
    if (isSkip && nestCntr > 0) {
        --nestCntr;
        return;
    }
    assertNoArg(it);
    condStatuses.pop();
    isSkip = false;
}

void PreprocessorImpl::directiveLine(string_constit_t &it) {
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
    par.ctx->warps.appendWarpLoc(globalLine, locations.top().line, locations.top().file);
}

void PreprocessorImpl::directiveError(string_constit_t &it) {
    if (isSkip)
        return;
    std::string msg = getStringArg(it, false);
    assertNoArg(it);
    printError(fmt::format("#error \"{}\"", msg)); // Exit here
}

void PreprocessorImpl::directiveWarning(string_constit_t &it) {
    if (isSkip)
        return;
    std::string msg = getStringArg(it, false);
    assertNoArg(it);
    printWarn(fmt::format("#warning \"{}\"", msg));
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
        else if (ident == "__TIME__") { // "15:24:12", "01:02:16"
            fmt::print(globalSS, "\"{:%H:%M:%S}\"", fmt::localtime(trTime));
            return;
        }
        else if (ident == "__TIMESTAMP__") { // "Sun Mar 27 15:24:12 2022"
            fmt::print(globalSS, "\"{:%a %b %e %H:%M:%S %Y}\"", fmt::localtime(trTime));
            return;
        }
        else if (ident == "__FILE_NAME__") {
            std::string const &filename = locations.top().file;
            fmt::print(globalSS, "\"{:%s}\"", basename(filename.c_str()));
            return;
        }
        else if (ident == "__BASE_FILE__") {
            fmt::print(globalSS, "\"{:%s}\"", baseFilename);
            return;
        }
        else if (ident == "__COUNTER__") {
            fmt::print(globalSS, "{}", usrCounter);
            usrCounter++;
            return;
        }
        else if (ident == "__VERSION__") {
            fmt::print(globalSS, "\"cw39 {}\"", CLIArgs::getSelfVersion());
            return;
        }
        else if (ident == "__INCLUDE_LEVEL__") {
            fmt::print(globalSS, "{}", raw.size() - 1);
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


Preprocessor::Preprocessor(std::string const &path, std::vector<std::string> const &rawDefines) {
    ctx = std::make_shared<ParsingContext>(path);
    addSystemDefines();
    parseRawDefines(rawDefines);
    PreprocessorImpl(*this, path);
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

std::shared_ptr<ParsingContext> Preprocessor::getContext() const {
    return ctx;
}

void Preprocessor::parseRawDefines(const std::vector<std::string> &rawDefines) {
    // TODO: check for alphanum names
    for (std::string const &s : rawDefines) {
        auto delimPos = s.find('=');
        if (delimPos != std::string::npos) {
            std::string name(s, 0, delimPos);
            std::string val(s, delimPos + 1, std::string::npos);
            addDefine(std::move(name), std::move(val));
        }
        else {
            addDefine(s, "");
        }
    }
}

void Preprocessor::addSystemDefines() {
    addDefine("__llvm__", "1");
    addDefine("__CW39__", "1");

#ifdef __unix__
    addDefine("__unix__", "1");
#endif
#ifdef __linux__
    addDefine("__linux__", "1");
#endif
#ifdef __sun // Solaris
    addDefine("__sun", "1");
#endif
#ifdef __FreeBSD__
    addDefine("__FreeBSD__", "1");
#endif
#ifdef __NetBSD__
    addDefine("__NetBSD__", "1");
#endif
#ifdef __OpenBSD__
    addDefine("__OpenBSD__", "1");
#endif
#ifdef __APPLE__
    addDefine("__APPLE__", "1");
#endif
#ifdef __hpux // HP-UX
    addDefine("__hpux", "1");
#endif
#ifdef __osf__ // Tru64 UNIX (formerly DEC OSF1)
    addDefine("__osf__", "1");
#endif
#ifdef __sgi // Irix
    addDefine("__sgi", "1");
#endif
#ifdef _AIX // AIX
    addDefine("_AIX", "1");
#endif
#ifdef _WIN32
    addDefine("_WIN32", "1");
#endif

    addDefine("__FILE__", "1");
    addDefine("__LINE__", "1");
    addDefine("__DATE__", "1");
    addDefine("__TIME__", "1");
    addDefine("__TIMESTAMP__", "1");
    addDefine("__FILE_NAME__", "1");
    addDefine("__BASE_FILE__", "1");
    addDefine("__COUNTER__", "1");
    addDefine("__VERSION__", "1");
    addDefine("__INCLUDE_LEVEL__", "1");
}

