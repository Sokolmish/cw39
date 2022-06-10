#ifndef PREPROCESSOR_HPP_INCLUDED__
#define PREPROCESSOR_HPP_INCLUDED__

#include <string>
#include <map>
#include <memory>

#include "parsing_context.hpp"

class Preprocessor {
public:
    static const constexpr size_t MAX_INCLUDES_DEPTH = 64;

    /** Raw defines in format "macro=value" */
    Preprocessor(std::string const &path, std::vector<std::string> const &rawDefines);

    void addDefine(std::string name, std::string value);
    void removeDefine(std::string const &name);

    std::string getText() const;
    std::shared_ptr<ParsingContext> getContext() const;

    auto moveData() && {
        return std::make_tuple(
                std::move(finalText),
                std::move(ctx));
    }

private:
    std::string finalText;
    std::shared_ptr<ParsingContext> ctx;

    std::map<std::string, std::string> defines;

    void parseRawDefines(std::vector<std::string> const &rawDefines);
    void addSystemDefines();

    friend class PreprocessorImpl;
};

#endif /* PREPROCESSOR_HPP_INCLUDED__ */
