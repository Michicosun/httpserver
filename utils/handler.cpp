#include <unistd.h>
#include <utils/types.h>
#include <utils/helpers.h>
#include <cstring>
#include <sstream>


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static std::string endline = "\r\n";

void makeRequest(std::stringstream& response, const std::string& body) {
    response 
        << "HTTP/1.1 200 OK" << endline
        << "Accept-Ranges: bytes" << endline
        << "Content-Length: " << body.size() << endline
        << endline
        << body;
}

void GETRequest(std::stringstream& response, const HttpRequest& request) {
    const char* path = request.path.c_str() + 1;    

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        makeRequest(response, std::strerror(errno));
        return;
    }

    std::string file_content;

    char c;
    while (read(fd, &c, sizeof(c)) == 1 && c != EOF) {
        file_content.push_back(c);
    }

    makeRequest(response, file_content);

    close(fd);
}

void POSTRequest(std::stringstream& response, const HttpRequest& request) {
    const char* path = request.path.c_str() + 1;    

    int fd = open(path, O_WRONLY | O_CREAT, 0600);
    if (fd < 0) {
        makeRequest(response, std::strerror(errno));
        return;
    }

    int cnt = 0;
    while (cnt < request.body.size()) {
        cnt += write(fd, request.body.c_str() + cnt, request.body.size() - cnt);
    }

    close(fd);

    makeRequest(response, "Done");
}

void PUTRequest(std::stringstream& response, const HttpRequest& request) {
    const char* path = request.path.c_str() + 1;    

    int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0600);
    if (fd < 0) {
        makeRequest(response, std::strerror(errno));
        return;
    }

    int cnt = 0;
    while (cnt < request.body.size()) {
        cnt += write(fd, request.body.c_str() + cnt, request.body.size() - cnt);
    }

    close(fd);

    makeRequest(response, "Done");
}

void DELETERequest(std::stringstream& response, const HttpRequest& request) {
    const char* path = request.path.c_str() + 1;    

    if (remove(path) < 0){
        makeRequest(response, std::strerror(errno));
        return;
    }

    makeRequest(response, "Done");
}

void UnknownRequest(std::stringstream& response, const HttpRequest& request) {
    makeRequest(response, (request.rtype + " Not Implemented").c_str());
}

void sendResponse(int connection, const HttpRequest& request) {
    std::stringstream response;

    if (request.rtype == "GET") {
        GETRequest(response, request);
    } 
    else if (request.rtype == "POST") {
        POSTRequest(response, request);
    }
    else if (request.rtype == "PUT") {
        PUTRequest(response, request);
    }
    else if (request.rtype == "DELETE") {
        DELETERequest(response, request);
    }
    else {
        UnknownRequest(response, request);
    }

    ssize_t res = send(connection, response.str().c_str(), response.str().size(), 0);
    if (res == -1) {
        throwError("send");
    }
}
