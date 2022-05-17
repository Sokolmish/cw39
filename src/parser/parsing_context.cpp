#include "parsing_context.hpp"
#include "utils.hpp"

ParsingContext::ParsingContext(std::string topFileName)
        : warps(topFileName) {
    reservedWords[getIdentId("__func__")] = RESW_FUNC_NAME;
    reservedWords[getIdentId("__builtin_ctz")] = RESW_BUILTIN_CTZ;
    reservedWords[getIdentId("__builtin_clz")] = RESW_BUILTIN_CLZ;
    reservedWords[getIdentId("__builtin_popcount")] = RESW_BUILTIN_POPCNT;
    reservedWords[getIdentId("__builtin_bitreverse32")] = RESW_BUILTIN_BITREV32;
}

string_id_t ParsingContext::getStringId(const char *str, size_t len, bool unescape) {
    std::string newStr;

    if (unescape) {
        newStr.reserve(len);

        for (size_t i = 1; i < len - 1; i++) {
            if (str[i] != '\\') {
                newStr.push_back(str[i]);
            }
            else {
                i++;
                if (str[i] == 'x') {
                    i++;
                    if (i < len - 2) {
                        newStr.push_back(parseXEscape(str[i], str[i + 1]));
                        i++;
                    }
                    else {
                        throw cw39_internal_error(fmt::format(
                                "Wrong escape sequence in string: {}", str));
                    }
                }
                else {
                    if (i < len - 1) {
                        newStr.push_back(unescapeChar(str[i]));
                    }
                    else {
                        throw cw39_internal_error(fmt::format(
                                "Wrong escape sequence in string: {}", str));
                    }
                }
            }
        }
    }
    else {
        newStr = std::string(str, len);
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

string_id_t ParsingContext::getStringId(std::string const &str, bool unescape) {
    return getStringId(str.c_str(), str.size(), unescape);
}

string_id_t ParsingContext::getIdentId(const char *ident, size_t len) {
    std::string str(ident, len);
    auto it = identsMap.lower_bound(str);
    if (it == identsMap.end() || it->first != str) {
        auto ins = identsMap.emplace_hint(it, std::move(str), IdentInfo(idCnt));
        invIdentsMap.emplace_hint(invIdentsMap.end(), idCnt, ins->first);
        idCnt++;
        return ins->second.id;
    }
    else { // If already exists
        return it->second.id;
    }
}

string_id_t ParsingContext::getIdentId(std::string const &str) {
    return getIdentId(str.c_str(), str.size());
}

std::string ParsingContext::getIdentById(string_id_t id) const {
    return std::string(invIdentsMap.find(id)->second);
}

std::string ParsingContext::getStringById(string_id_t id) const {
    return std::string(invStringsMap.find(id)->second);
}

std::optional<ParsingContext::ReservedWords> ParsingContext::getReserved(string_id_t id) const {
    auto it = reservedWords.find(id);
    if (it == reservedWords.end())
        return {};
    return it->second;
}

bool ParsingContext::isIntrinsicFuncName(string_id_t id) const {
    auto word = getReserved(id);
    if (!word.has_value())
        return false;
    return isInList(word, RESW_BUILTIN_CTZ, RESW_BUILTIN_CLZ,
                    RESW_BUILTIN_POPCNT, RESW_BUILTIN_BITREV32);
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
