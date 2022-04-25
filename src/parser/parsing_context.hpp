#ifndef PARSING_CONTEXT_HPP_INCLUDED__
#define PARSING_CONTEXT_HPP_INCLUDED__

#include <string>
#include <vector>
#include <map>

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
        // TODO: duplicates IdentType from common.hpp
        enum : int { IDENT, DEF_TYPE } type = IDENT;
        explicit IdentInfo(string_id_t id) : id(id) {}
    };

    struct StringInfo {
        string_id_t id;
        explicit StringInfo(string_id_t id) : id(id) {}
    };

    ParsingContext(std::string topFileName);

    string_id_t getStringId(const char *str, size_t len);
    string_id_t getStringId(std::string const &str);

    string_id_t getIdentId(const char *str, size_t len, IdentType *type);
    string_id_t getIdentId(std::string const &str, IdentType *type);

    std::string getIdentById(string_id_t id) const;

    std::string getStringById(string_id_t id) const;

    LinesWarpMap warps;

private:
    string_id_t idCnt = 1;
    string_id_t strCnt = 1;

    std::map<std::string, IdentInfo, std::less<>> identsMap;
    std::map<std::string, StringInfo, std::less<>> stringsMap;

    std::map<string_id_t, std::string> invIdentsMap;
    std::map<string_id_t, std::string> invStringsMap;
};

#endif /* PARSING_CONTEXT_HPP_INCLUDED__ */