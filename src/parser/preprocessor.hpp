#ifndef __PREPROCESSOR_HPP__
#define __PREPROCESSOR_HPP__

#include <string>
#include <map>
#include <stack>
#include <sstream>
#include <ctime>

#include "warps.hpp"

class Preprocessor {
public:
    Preprocessor(std::string const &path);

    void addDefine(std::string name, std::string value);
    void removeDefine(std::string const &name);

    std::string getText() const;
    LinesWarpMap getWarps() const;

    static const constexpr size_t MAX_INCLUDES_DEPTH = 64;

private:
    std::stack<std::string> raw;
    std::map<std::string, std::string> defines;

    size_t globalLine;

    std::stringstream globalSS;
    std::string finalText;

    LinesWarpMap warps;

    bool isSkip;
    int nestCntr;

    enum LastCondState { PC_IF_TRUE, PC_IF_FALSE, PC_ELSE };
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

#endif /* __PREPROCESSOR_HPP__ */
