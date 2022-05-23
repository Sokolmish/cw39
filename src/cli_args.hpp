#ifndef CLI_ARGS_HPP_INCLUDED__
#define CLI_ARGS_HPP_INCLUDED__

#include <string>
#include <vector>
#include <memory>
#include <optional>

class CLIArgs {
public:
    CLIArgs(int argc, char **argv);

    std::optional<std::string> getOutParam(const char *name) const;
    std::optional<std::string> outPreproc() const;
    std::optional<std::string> outAST() const;
    std::optional<std::string> outIR() const;
    std::optional<std::string> outCFG() const;
    std::optional<std::string> outLLVM() const;
    std::optional<std::string> outBC() const;
    std::optional<std::string> outASM() const;

    uint getOptLevel() const;
    std::vector<std::string> getDefines() const;
    bool isS1_Enabled() const;
    bool isS2_Enabled() const;

    bool isScannerTracing() const;
    bool isParserTracing() const;
    bool isShowTimes() const;

    std::string inputFile() const;

    std::string get_llc_name() const;

    ~CLIArgs(); // For unique_ptr to implementation
    class CLIArgs_Impl;

private:
    std::string getString(std::string const &name) const;

    friend class CLIArgs_Impl;
    std::unique_ptr<CLIArgs_Impl> impl;
};

#endif /* CLI_ARGS_HPP_INCLUDED__ */
