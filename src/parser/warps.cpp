#include "warps.hpp"

LinesWarpMap::LinesWarpMap(std::string const newFile) {
    ldata.reserve(16);
    ldata.push_back(LineWarp{
            .oldLine = 0,
            .newLine = 0,
            .newFile = getFilenum(newFile),
    });
}

void LinesWarpMap::appendWarpLoc(int oldLine, int newLine, std::string const newFile) {
    if (!ldata.empty() && oldLine <= ldata.back().oldLine)
        throw;
    ldata.push_back(LineWarp{
            .oldLine = oldLine,
            .newLine = newLine,
            .newFile = getFilenum(newFile),
    });
}

LinesWarpMap::Location LinesWarpMap::getLoc(int line) const {
    if (hint == ldata.size() - 1) {
        if (line < ldata.back().oldLine)
            return getLocHard(line);
        else
            return Location{
                    .filenum = ldata.back().newFile,
                    .line = ldata.back().newLine + line - ldata.back().oldLine };
    }
    else if (ldata[hint].oldLine < line && line < ldata[hint + 1].oldLine) {
        return Location{
                .filenum = ldata[hint].newFile,
                .line = ldata[hint].newLine + line - ldata[hint].oldLine };
    }
    else if (ldata[hint + 1].oldLine <= line) {
        while (hint < ldata.size() - 1 && ldata[hint + 1].oldLine <= line)
            hint++;
        return Location{
                .filenum = ldata[hint].newFile,
                .line = ldata[hint].newLine + line - ldata[hint].oldLine };
    }
    else {
        return getLocHard(line);
    }
}

LinesWarpMap::Location LinesWarpMap::getLocHard(int line) const {
    (void)line;
    throw; // TODO
}

std::string LinesWarpMap::getFilename(int num) const {
    for (auto &[file, filenum] : filenames)
        if (filenum == num)
            return file;
    return "<unknown_file>";
}

int LinesWarpMap::getFilenum(const std::string &file) {
    auto it = filenames.lower_bound(file);
    if (it->first == file)
        return it->second;
    int newNum = filenames.size();
    filenames.emplace_hint(it, file, newNum);
    return newNum;
}
