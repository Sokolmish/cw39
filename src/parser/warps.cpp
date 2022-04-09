#include "warps.hpp"

LinesWarpMap::LinesWarpMap(std::string const newFile) {
    ldata.reserve(64); // Preallocate some space
    ldata.push_back(LineWarp{
            .oldLine = 0,
            .newLine = 0,
            .newFile = getFilenum(newFile),
    });
}

void LinesWarpMap::appendWarpLoc(int oldLine, int newLine, std::string const newFile) {
    if (oldLine <= ldata.back().oldLine)
        throw;
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
