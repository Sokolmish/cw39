#ifndef __PREPROCESSOR_HPP__
#define __PREPROCESSOR_HPP__

#include <string>
#include <map>
#include <stack>

class Preprocessor {
public:
    explicit Preprocessor(std::string const &path);

    void addDefine(std::string name, std::string value);
    void removeDefine(std::string const &name);

    std::string process();

private:
    std::string raw;
    std::map<std::string, std::string> defines;

    bool isLineStart;
    bool isSkip;
    int nestCntr;

    enum LastCondState { PC_IF_TRUE, PC_IF_FALSE, PC_ELSE };
    std::stack<LastCondState> condStatuses;

    int curLine;

    using string_constit_t = decltype(raw.cbegin());

    void processDirective(std::string const &dir, string_constit_t &it);

    std::string getStringArg(string_constit_t &it, bool angleBrackets);
    uint32_t getU32Arg(string_constit_t &it);
    std::string getIdentArg(string_constit_t &it);
    void skipSpaces(string_constit_t &it);
    void assertNoArg(string_constit_t &it);

    void printError(std::string const &msg);
    void printWarn(std::string const &msg);

};

#endif /* __PREPROCESSOR_HPP__ */
