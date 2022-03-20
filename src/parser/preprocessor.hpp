#ifndef __PREPROCESSOR_HPP__
#define __PREPROCESSOR_HPP__

#include <string>
#include <map>
#include <vector>
#include <stack>
#include <sstream>

class LinesWarpMap {
public:
    explicit LinesWarpMap(std::string const newFile);

    struct Location {
        int filenum, line;
    };

    void appendWarpLoc(int oldLine, int newLine, std::string const newFile);
    Location getLoc(int line) const;

    std::string getFilename(int num) const;

private:
    struct LineWarp {
        int oldLine, newLine, newFile;
    };

    std::vector<LineWarp> ldata;
    std::map<std::string, int> filenames;

    mutable size_t hint = 0;

    int getFilenum(std::string const &file);
    Location getLocHard(int line) const;
};

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

    size_t globalLine = 0;

    std::stringstream globalSS;
    std::string finalText;

    LinesWarpMap warps;

    bool isLineStart;
    bool isSkip;
    int nestCntr;

    enum LastCondState { PC_IF_TRUE, PC_IF_FALSE, PC_ELSE };
    std::stack<LastCondState> condStatuses;

    struct Location {
        std::string file;
        int line;
    };
    std::stack<Location> locations;

    using string_constit_t = decltype(raw.top().cbegin());

    void processFile(std::string const &path);
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
