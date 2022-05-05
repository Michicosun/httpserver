#include <utils/helpers.h>
#include <unistd.h>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>

std::string readAllHeaders(std::size_t fd) {
    std::string request;
    
    char c;
    while (read(fd, &c, 1) == 1 && c != EOF) {
        request.push_back(c);        

        if (
            request.size() >= 4 && 
            request.substr(request.size() - 4) == "\r\n\r\n") break; 
    }

    return request.substr(0, request.size() - 2);
}

void parseLine(const std::string& line, HttpRequest& req) {
    if (!req.correct) {
        return;
    }

    int colon_index = line.find(":");

    if (colon_index == std::string::npos) {
        req.correct = false;
        return;
    } 

    std::string key = line.substr(0, colon_index);
    std::string value = line.substr(colon_index + 1, line.size() - colon_index - 2);

    req.headers[key] = value;
}

void parseFirstLine(const std::string& line, HttpRequest& req) {
    char* method = nullptr;
    char* path = nullptr;
    char* protocol = nullptr;

    if (sscanf(line.c_str(), "%ms %ms %ms", &method, &path, &protocol) != 3) {
        delete method;
        delete path;
        delete protocol;
        return;
    }

    req.rtype = std::string(method);
    req.path = std::string(path);
    req.proto = std::string(protocol);
}

HttpRequest parseRequest(int fd) {
    std::string s = readAllHeaders(fd);

    HttpRequest req = {.correct = true};

    std::stringstream ss(s);
    std::string line;

    std::getline(ss, line);
    parseFirstLine(line, req);

    while (std::getline(ss, line) && !line.empty()) {
        parseLine(line, req);
    }

    auto it = req.headers.find("Content-Length");
    if (it != req.headers.end()) {
        std::size_t body_len = std::stoi(it->second);

        while (body_len > 0) {
            char c;
            int err = read(fd, &c, sizeof(c));

            if (err < 0) {
                req.correct = false;
                break;
            } else if (err == 1) {
                req.body.push_back(c);
                --body_len;
            }
        }
    }

    return req;
}

