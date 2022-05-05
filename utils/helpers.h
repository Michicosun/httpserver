#pragma once

#include <unistd.h>
#include <cstring>
#include <string>
#include <cerrno>
#include <iostream>

#include <utils/types.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

struct HttpServerError {
    std::string why;
};

[[noreturn]] void throwError(std::string pref);

template <class... Args>
void log(const char* format_s, Args... args) {
    char write_str[100] = {0};

    sprintf(write_str, format_s, args...);
    write(STDOUT_FILENO, write_str, strlen(write_str));
}

void log(const char* format_s);

HttpRequest parseRequest(int fd);

void sendResponse(int fd, const HttpRequest& reques);
