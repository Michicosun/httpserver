#include <utils/types.h>
#include <utils/helpers.h>
#include <utils/fd_handler.h>

#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>

static std::string endline = "\r\n";

void makeResponse(std::stringstream& response, const std::string& body) {
    response 
        << "HTTP/1.1 200 OK" << endline
        << "Content-Type: text/plain; charset=utf-8" << endline
        << "Accept-Ranges: bytes" << endline
        << "Content-Length: " << body.size() << endline
        << endline
        << body;
}

void GETRequest(std::stringstream& response, const HttpRequest& request) {
    const char* path = request.path.c_str() + 1;    

    FdHandler fd(open(path, O_RDONLY));
    if (fd.getFd() < 0) {
        makeResponse(response, std::strerror(errno));
        return;
    }

    std::string file_content;

    char c;
    while (read(fd.getFd(), &c, sizeof(c)) == 1 && c != EOF) {
        file_content.push_back(c);
    }

    makeResponse(response, file_content);
}

void POSTRequest(std::stringstream& response, const HttpRequest& request) {
    const char* path = request.path.c_str() + 1;    

    FdHandler fd(open(path, O_WRONLY | O_CREAT | O_TRUNC, 0600));
    if (fd.getFd() < 0) {
        makeResponse(response, std::strerror(errno));
        return;
    }

    int cnt = 0;
    while (cnt < request.body.size()) {
        cnt += write(fd.getFd(), request.body.c_str() + cnt, request.body.size() - cnt);
    }

    makeResponse(response, "Done");
}

void PUTRequest(std::stringstream& response, const HttpRequest& request) {
    const char* path = request.path.c_str() + 1;    

    FdHandler fd(open(path, O_WRONLY | O_CREAT | O_APPEND, 0600));
    if (fd.getFd() < 0) {
        makeResponse(response, std::strerror(errno));
        return;
    }

    int cnt = 0;
    while (cnt < request.body.size()) {
        cnt += write(fd.getFd(), request.body.c_str() + cnt, request.body.size() - cnt);
    }

    makeResponse(response, "Done");
}

void DELETERequest(std::stringstream& response, const HttpRequest& request) {
    const char* path = request.path.c_str() + 1;    

    if (remove(path) < 0){
        makeResponse(response, std::strerror(errno));
        return;
    }

    makeResponse(response, "Done");
}

void UnknownRequest(std::stringstream& response, const HttpRequest& request) {
    makeResponse(response, (request.rtype + " Not Implemented").c_str());
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
