#ifndef EXT_PROCESS_HPP_INCLUDED__
#define EXT_PROCESS_HPP_INCLUDED__

#include <string>
#include <vector>
#include <memory>

class ExtProcess {
public:
    ExtProcess(std::string name, std::vector<std::string> const &args);

    void wait();

    void sendString(std::string const &str);
    void closeFd();

    ~ExtProcess(); // Needed for unique_ptr to incomplete type

    struct InternalData;

private:
    std::string name;
    std::vector<std::string> args;
    std::unique_ptr<InternalData> data;

    void createProcess();
};


#endif /* EXT_PROCESS_HPP_INCLUDED__ */
