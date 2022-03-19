#ifndef __PREPROCESSOR_HPP__
#define __PREPROCESSOR_HPP__

#include <string>
#include <map>
#include <stack>

class Preprocessor {
public:
    explicit Preprocessor(std::string const &path);

    void addDefine(std::string name, std::string value);
    void removeDefine(std::string const &name);

    std::string process();

private:
    std::string raw;
    std::map<std::string, std::string> defines;

    bool isLineStart;
    bool isSkip;
    int nestCntr;

    enum LastCondState { PC_IF_TRUE, PC_IF_FALSE, PC_ELSE };
    std::stack<LastCondState> cond_statuses;

    using string_constit_t = decltype(raw.cbegin());

    void process_directive(std::string const &dir, string_constit_t &it);
    std::string get_directive_arg(string_constit_t &it);
    void assert_no_directive_arg(string_constit_t &it);
    std::string get_ident(string_constit_t &it);
    void skip_spaces(string_constit_t &it);
};

#endif /* __PREPROCESSOR_HPP__ */
