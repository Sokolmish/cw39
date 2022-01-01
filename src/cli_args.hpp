#ifndef __CLI_ARGS_HPP__
#define __CLI_ARGS_HPP__

#include <string>
#include <vector>
#include <memory>

class CLIArgs {
public:
    CLIArgs(int argc, char **argv);

    std::vector<std::string> unmatched() const;
    size_t count(std::string const &name) const;
    std::string getString(std::string const &name) const;

    ~CLIArgs(); // For unique_ptr to implementation
    class CLIArgs_Impl;

private:
    friend class CLIArgs_Impl;
    std::unique_ptr<CLIArgs_Impl> impl;
};


#endif /* __CLI_ARGS_HPP__ */
