#pragma once

class FdHandler {
private:
    int fd;

public:
    FdHandler(int fd);

    int getFd();

    ~FdHandler();
};

