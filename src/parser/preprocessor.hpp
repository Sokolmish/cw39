#ifndef PREPROCESSOR_HPP_INCLUDED__
#define PREPROCESSOR_HPP_INCLUDED__

#include <string>
#include <map>
#include <memory>

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
    std::string finalText;
    std::map<std::string, std::string> defines;
    std::unique_ptr<LinesWarpMap> warps;
};

#endif /* PREPROCESSOR_HPP_INCLUDED__ */
