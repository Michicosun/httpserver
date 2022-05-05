#pragma once

#include <string>
#include <unordered_map>

#include <sys/socket.h>

struct HttpRequest {
    std::string rtype;
    std::string path;
    std::string proto;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    bool correct;
};