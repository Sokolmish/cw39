#include "ext_process.hpp"
#include "utils.hpp"
#include <unistd.h>
#include <sys/wait.h>

struct ExtProcess::InternalData {
    pid_t pid;
    int fd;
};

ExtProcess::ExtProcess(std::string name, const std::vector<std::string> &args)
        : name(std::move(name)), args(std::move(args)), data(std::make_unique<InternalData>()) {
    createProcess();
}

ExtProcess::~ExtProcess() = default;

void ExtProcess::createProcess() {
    int fd[2];
    if (pipe(fd) != 0)
        throw cw39_error(fmt::format("pipe: {}", strerror(errno)));

    data->pid = fork();
    if (data->pid < 0)
        throw cw39_error(fmt::format("fork: {}", strerror(errno)));

    if (data->pid == 0) { // Child process
        close(fd[1]); // Close unused write end

        if (dup2(fd[0], STDIN_FILENO) < 0)
            throw cw39_error(fmt::format("dup2: {}", strerror(errno)));

        close(fd[0]);

        auto argv = std::make_unique<char *[]>(args.size() + 1);
        argv[0] = const_cast<char *>(name.c_str());
        for (size_t i = 0; i < args.size(); i++)
            argv[i + 1] = const_cast<char *>(args[i].c_str());

        if (execvp(name.c_str(), argv.get()) < 0)
            throw cw39_error(fmt::format("execvp '{}': {}", name, strerror(errno)));
    }

    // Parent process

    close(fd[0]); // Close unused read end
    data->fd = fd[1];
}

void ExtProcess::wait() {
    int wstatus;

    if (waitpid(data->pid, &wstatus, 0) < 0) {
        throw cw39_error(fmt::format("waitpid: {}", strerror(errno)));
    }
}

void ExtProcess::sendString(const std::string &str) {
    size_t total = 0;
    do {
        int wr = write(data->fd, str.c_str(), str.size());
        if (wr < 0)
            throw cw39_error(fmt::format("write to pipe: {}\n", strerror(errno)));
        total += wr;
    } while (total < str.size());
}

void ExtProcess::closeFd() {
    close(data->fd);
}
