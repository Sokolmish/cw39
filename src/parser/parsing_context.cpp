#include "parsing_context.hpp"
#include "utils.hpp"

ParsingContext::ParsingContext(std::string topFileName)
        : warps(topFileName) {}

static char unescapeChar(char ch) {
    // TODO: '\xhh'
    switch (ch) {
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 'f':
            return '\f';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 't':
            return '\t';
        case 'v':
            return '\v';
        case '\\':
            return '\\';
        case '\'':
            return '\'';
        case '\"':
            return '\"';
        case '?':
            return '\?';
        default:
            return ch;
    }
}

// TODO: possible bug with concatenated literals and escaping (leave unescaping in lexer only)
string_id_t ParsingContext::getStringId(const char *str, size_t len) {
    std::string newStr;
    newStr.reserve(len);

    for (size_t i = 1; i < len - 1; i++) {
        if (str[i] != '\\') {
            newStr.push_back(str[i]);
        }
        else {
            i++;
            if (i < len - 1)
                newStr.push_back(unescapeChar(str[i]));
        }
    }

    auto it = stringsMap.lower_bound(newStr);
    if (it == stringsMap.end() || it->first != newStr) {
        auto ins = stringsMap.emplace_hint(it, newStr, StringInfo(strCnt));
        invStringsMap.emplace_hint(invStringsMap.end(), strCnt, ins->first);
        strCnt++;
        return ins->second.id;
    }
    else { // If already exists
        return it->second.id;
    }
}

string_id_t ParsingContext::getStringId(std::string const &str) {
    return getStringId(str.c_str(), str.size());
}

string_id_t ParsingContext::getIdentId(const char *ident, size_t len, IdentType *type) {
    std::string str(ident, len);
    auto it = identsMap.lower_bound(str);
    if (it == identsMap.end() || it->first != str) {
        auto ins = identsMap.emplace_hint(it, std::move(str), IdentInfo(idCnt));
        invIdentsMap.emplace_hint(invIdentsMap.end(), idCnt, ins->first);
        idCnt++;
        *type = IdentType::IDENT;
        return ins->second.id;
    }
    else { // If already exists
        bool isIdent = (it->second.type == IdentInfo::IDENT);
        *type = isIdent ? IdentType::IDENT : IdentType::TYPENAME;
        return it->second.id;
    }
}

string_id_t ParsingContext::getIdentId(std::string const &str, IdentType *type) {
    return getIdentId(str.c_str(), str.size(), type);
}

std::string ParsingContext::getIdentById(string_id_t id) const {
    return std::string(invIdentsMap.find(id)->second);
}

std::string ParsingContext::getStringById(string_id_t id) const {
    return std::string(invStringsMap.find(id)->second);
}


LinesWarpMap::LinesWarpMap(std::string newFile) {
    ldata.reserve(64); // Preallocate some space

    int rootFilenum = 0;
    filenames.emplace_hint(filenames.end(), std::move(newFile), rootFilenum);

    ldata.push_back(LineWarp{
            .oldLine = 0,
            .newLine = 0,
            .newFile = rootFilenum,
    });
}

void LinesWarpMap::appendWarpLoc(int oldLine, int newLine, std::string const newFile) {
    if (oldLine <= ldata.back().oldLine)
        throw cw39_internal_error("Warp loc can be appended only at the end");
    ldata.push_back(LineWarp{
            .oldLine = oldLine,
            .newLine = newLine,
            .newFile = getFilenum(newFile),
    });
}

LinesWarpMap::Location LinesWarpMap::getLoc(int line) const {
    LineWarp needle;
    needle.oldLine = line;
    auto comp = [](auto const &a, auto const &b) {
        return a.oldLine < b.oldLine;
    };
    auto lbIt = std::lower_bound(ldata.cbegin(), ldata.cend(), needle, comp);

    if (lbIt->oldLine == line || lbIt == ldata.cbegin()) {
        int newline = lbIt->newLine + line - lbIt->oldLine;
        return { .filenum = lbIt->newFile, .line = newline };
    }
    else {
        --lbIt;
        int newline = lbIt->newLine + line - lbIt->oldLine;
        return { .filenum = lbIt->newFile, .line = newline };
    }
}

std::string LinesWarpMap::getFilename(int num) const {
    for (auto &[file, filenum] : filenames)
        if (filenum == num)
            return file;
    return "<unknown_file>";
}

int LinesWarpMap::getFilenum(const std::string &file) {
    auto it = filenames.lower_bound(file);
    if (it != filenames.end() && it->first == file)
        return it->second;
    int newNum = filenames.size();
    filenames.emplace_hint(it, file, newNum);
    return newNum;
}
