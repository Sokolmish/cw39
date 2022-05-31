#include "ext_process.hpp"
#include "utils.hpp"
#include <unistd.h>
#include <sys/wait.h>

struct SysPipe {
    int readFd, writeFd;

    SysPipe() {
        int fds[2];
        if (pipe(fds) != 0)
            throw cw39_error(fmt::format("pipe: {}", strerror(errno)));
        readFd = fds[0];
        writeFd = fds[1];
    }

    /** Redirect read-end of the pipe into destFd (e.g. stdin). */
    void ejectTo(int destFd) {
        if (dup2(readFd, destFd) < 0)
            throw cw39_error(fmt::format("dup2: {}", strerror(errno)));
        close(readFd);
    }

    /** Redirect write-end of the pipe to srcFd. */
    void injectFrom(int srcFd) {
        if (dup2(writeFd, srcFd) < 0)
            throw cw39_error(fmt::format("dup2: {}", strerror(errno)));
        close(writeFd);
    }

    void closeReadEnd() {
        close(readFd);
    }

    void closeWriteEnd() {
        close(writeFd);
    }
};


struct ExtProcess::InternalData {
    pid_t pid;
    int inputFd;
    int outputFd;
};

ExtProcess::ExtProcess(std::string name, const std::vector<std::string> &args)
        : name(std::move(name)), args(std::move(args)), data(std::make_unique<InternalData>()) {
}

ExtProcess::~ExtProcess() = default;

void ExtProcess::start() {
    SysPipe sendPipe;

    data->pid = fork();
    if (data->pid < 0)
        throw cw39_error(fmt::format("fork: {}", strerror(errno)));

    if (data->pid == 0) { // Child process
        sendPipe.closeWriteEnd();
        sendPipe.ejectTo(STDIN_FILENO);

        auto argv = std::make_unique<char *[]>(args.size() + 2);
        argv[0] = const_cast<char *>(name.c_str());
        for (size_t i = 0; i < args.size(); i++)
            argv[i + 1] = const_cast<char *>(args[i].c_str());
        argv[args.size()] = nullptr;

        if (execvp(name.c_str(), argv.get()) < 0)
            throw cw39_error(fmt::format("execvp '{}': {}", name, strerror(errno)));
    }

    // Parent process

    sendPipe.closeReadEnd();
    data->inputFd = sendPipe.writeFd;
}

void ExtProcess::join() {
    int wstatus;
    if (waitpid(data->pid, &wstatus, 0) < 0) {
        throw cw39_error(fmt::format("waitpid: {}", strerror(errno)));
    }
}

void ExtProcess::sendString(const std::string &str) {
    size_t total = 0;
    do {
        int wr = write(data->inputFd, str.c_str(), str.size());
        if (wr < 0)
            throw cw39_error(fmt::format("write to pipe: {}\n", strerror(errno)));
        total += wr;
    } while (total < str.size());
}

void ExtProcess::closeInputFd() {
    close(data->inputFd);
}

int ExtProcess::getOutputFd() const {
    return data->outputFd;
}

int ExtProcess::getInputFd() const {
    return data->inputFd;
}

void ExtProcess::setOutputToStdout() {
    outputToStdout = false;
}
