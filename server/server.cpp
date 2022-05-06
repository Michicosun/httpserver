#include <server/server.h>
#include <utils/helpers.h>
#include <utils/logger.h>

#include <sys/signalfd.h>
#include <sys/epoll.h>


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

HttpServer::HttpServer(size_t port, sigset_t mask, size_t backlog) : 
    pool(std::thread::hardware_concurrency()), 
    backlog{backlog}, 
    port{port},
    mask{mask} 
{
    signal_fd = signalfd(-1, &mask, 0);

    if (signal_fd < 0) {
        throwError("signalfd");
    }
}

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

    log << GRN "connected from:" RESET 
        " address: "<< host << 
        ", port: " << port << 
        " -> connection: "
        << connection << Logger::endl;

    return connection; 
}

bool HttpServer::run() {
    create_listener(port);

    int efd = epoll_create1(0);

    struct epoll_event eev = {};
    eev.events = EPOLLIN;
    eev.data.fd = signal_fd;
    epoll_ctl(efd, EPOLL_CTL_ADD, signal_fd, &eev);

    eev.events = EPOLLIN;
    eev.data.fd = lfd;
    epoll_ctl(efd, EPOLL_CTL_ADD, lfd, &eev);    

    while (true) {
        int ready_cnt = epoll_pwait(efd, &eev, 2, 1000, &mask);
        if (ready_cnt > 0) {
            int ready_fd = eev.data.fd;
            if (ready_fd == lfd) {
                std::size_t con = accept_connection();
                pool.submit(con);
            } else {
                log << RED "\rexit initialized -> joining threads..." RESET << Logger::endl;
                shutdown();
                return true;
            }
        }
    }

    return false;
}

void HttpServer::shutdown() {
    pool.join();
}