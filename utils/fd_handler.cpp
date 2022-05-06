#include <utils/fd_handler.h>

#include <unistd.h>

FdHandler::FdHandler(int fd) : fd{fd} {}

int FdHandler::getFd() {
    return fd;
}

FdHandler::~FdHandler() {
    if (fd >= 0) {
        close(fd);
    }
}

