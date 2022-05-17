#ifndef PARSING_CONTEXT_HPP_INCLUDED__
#define PARSING_CONTEXT_HPP_INCLUDED__

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <optional>

#include "common.hpp"

class LinesWarpMap {
public:
    explicit LinesWarpMap(std::string newFile);

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

    int getFilenum(std::string const &file);
};

class ParsingContext {
public:
    struct IdentInfo {
        string_id_t id;
        IdentInfo(string_id_t id) : id(id) {}
    };

    struct StringInfo {
        string_id_t id;
        explicit StringInfo(string_id_t id) : id(id) {}
    };

    ParsingContext(std::string topFileName);

    string_id_t getStringId(const char *str, size_t len, bool unescape);
    string_id_t getStringId(std::string const &str, bool unescape);

    string_id_t getIdentId(const char *str, size_t len);
    string_id_t getIdentId(std::string const &str);

    std::string getIdentById(string_id_t id) const;

    std::string getStringById(string_id_t id) const;

    LinesWarpMap warps;

    enum ReservedWords {
        RESW_FUNC_NAME,
        RESW_BUILTIN_CTZ, RESW_BUILTIN_CLZ,
        RESW_BUILTIN_POPCNT, RESW_BUILTIN_BITREV32,
    };

    std::optional<ReservedWords> getReserved(string_id_t id) const;
    bool isIntrinsicFuncName(string_id_t id) const;

private:
    string_id_t idCnt = 1;
    string_id_t strCnt = 1;

    std::map<std::string, IdentInfo, std::less<>> identsMap;
    std::map<std::string, StringInfo, std::less<>> stringsMap;

    std::map<string_id_t, std::string> invIdentsMap;
    std::map<string_id_t, std::string> invStringsMap;

    std::unordered_map<string_id_t, ReservedWords> reservedWords;
};

#endif /* PARSING_CONTEXT_HPP_INCLUDED__ */
