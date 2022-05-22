#include <utils/helpers.h>
#include <sstream>

std::string readAllHeaders(int fd) {
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

void parseLine(const std::string& line, HttpRequest& request) {
    if (!request.correct) {
        return;
    }

    auto colon_index = line.find(":");

    if (colon_index == std::string::npos) {
        request.correct = false;
        return;
    } 

    std::string key = line.substr(0, colon_index);
    std::string value = line.substr(colon_index + 1, line.size() - colon_index - 2);

    request.headers[key] = value;
}

void parseFirstLine(const std::string& line, HttpRequest& request) {
    std::stringstream words(line);

    words >> request.rtype >> request.path >> request.proto;

    if (request.rtype.empty() || request.path.empty() || request.proto.empty()) {
        request.correct = false;
    }
}

HttpRequest parseRequest(int fd) {
    std::string s = readAllHeaders(fd);

    HttpRequest request = {.correct = true};

    std::stringstream ss(s);
    std::string line;

    std::getline(ss, line);
    parseFirstLine(line, request);

    while (std::getline(ss, line) && !line.empty()) {
        parseLine(line, request);
    }

    auto it = request.headers.find("Content-Length");
    if (it != request.headers.end()) {
        std::size_t body_len = std::stoi(it->second);

        while (body_len > 0) {
            char c;
            int err = read(fd, &c, sizeof(c));

            if (err < 0) {
                request.correct = false;
                break;
            } else if (err == 1) {
                request.body.push_back(c);
                --body_len;
            }
        }
    }

    return request;
}

