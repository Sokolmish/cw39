#ifndef EXT_PROCESS_HPP_INCLUDED__
#define EXT_PROCESS_HPP_INCLUDED__

#include <string>
#include <vector>
#include <memory>

class ExtProcess {
public:
    ExtProcess(std::string name, std::vector<std::string> const &args);

    void start();

    int getInputFd() const;
    int getOutputFd() const;

    void setOutputToStdout();

    void join(); // calls wait() for process

    void sendString(std::string const &str);
    void closeInputFd();

    ~ExtProcess(); // Needed for unique_ptr to incomplete type

    struct InternalData;

private:
    std::string name;
    std::vector<std::string> args;
    std::unique_ptr<InternalData> data;

    bool outputToStdout = false;
};

#endif /* EXT_PROCESS_HPP_INCLUDED__ */
