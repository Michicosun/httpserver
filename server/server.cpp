#include <netinet/in.h>
#include <server/server.h>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <cerrno>

#include <sys/socket.h> 
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

void HttpServer::throwError(std::string pref) {
    char* err = std::strerror(errno);
    std::string serr(err);
    throw HttpServerError{pref + ": " + serr};
} 

void HttpServer::create_listener(std::size_t port) {
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throwError("socket");
    }

    int sopt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &sopt, sizeof(sopt)) < 0) {
        throwError("setsockopt");
    }

    struct sockaddr_in baddr = {0};
    baddr.sin_family = AF_INET;
    baddr.sin_port = htons(port);
    baddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, reinterpret_cast<struct sockaddr*>(&baddr), sizeof(baddr)) < 0) {
        throwError("bind");
    }

    if (listen(fd, backlog) < 0) {
        throwError("listen");
    }
    
    lfd = fd;
}



HttpServer::HttpServer(size_t port, size_t backlog) : 
    pool(std::thread::hardware_concurrency()), 
    backlog{backlog}, 
    port{port} {}

std::size_t HttpServer::accept_connection() {
    struct sockaddr_storage client;
    socklen_t len = sizeof(client);
    int connection = accept(lfd, reinterpret_cast<struct sockaddr*>(&client), &len);

    char host[100], port[10];
    getnameinfo(
        reinterpret_cast<struct sockaddr*>(&client), 
        len,
        host, 
        sizeof(host), 
        port, 
        sizeof(port),
        NI_NUMERICHOST | NI_NUMERICSERV
    );

    char write_str[120] = {0};
    sprintf(write_str, "connected from: address: %s, port: %s\n", host, port);
    write(STDOUT_FILENO, write_str, strlen(write_str));
    
    return connection; 
}

void HttpServer::run() {
    create_listener(port);

    while (std::size_t con = accept_connection()) {
        pool.submit(con);
    }    

    exit(0);
}
