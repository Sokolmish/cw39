#ifndef PREPROCESSOR_HPP_INCLUDED__
#define PREPROCESSOR_HPP_INCLUDED__

#include <string>
#include <map>
#include <memory>

#include "parsing_context.hpp"

class Preprocessor {
public:
    Preprocessor(std::string const &path);

    void addDefine(std::string name, std::string value);
    void removeDefine(std::string const &name);

    std::string getText() const;
    std::shared_ptr<ParsingContext> getContext() const;

    static const constexpr size_t MAX_INCLUDES_DEPTH = 64;

private:
    std::string finalText;
    std::shared_ptr<ParsingContext> ctx;

    std::map<std::string, std::string> defines;

    void addSystemDefines();

    friend class PreprocessorImpl;
};

#endif /* PREPROCESSOR_HPP_INCLUDED__ */
