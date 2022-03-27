#ifndef WARPS_HPP_INCLUDED__
#define WARPS_HPP_INCLUDED__

#include <vector>
#include <string>
#include <map>

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

#endif /* WARPS_HPP_INCLUDED__ */
