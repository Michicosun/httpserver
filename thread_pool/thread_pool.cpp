#include <thread_pool/thread_pool.h>
#include <server/server.h>

#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstring>

#include <sys/socket.h> 
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

ThreadPool::ThreadPool(std::size_t thread_count) : queue_(std::chrono::seconds(1)) {
    for (int i = 0; i < thread_count; ++i) {
        threads_.emplace_back([this] () {
            WorkRoutine(); 
        });
    }
}

void ThreadPool::submit(std::size_t fd) {
    queue_.push(fd);
}

void ThreadPool::join() {
    need_stop = true;
}

void process_client(std::size_t fd) {
    char c;
    while (read(fd, &c, 1) == 1 && c != EOF) {
        write(STDOUT_FILENO, &c, 1);
    }
    
    struct sockaddr_in client = {};
    socklen_t len = sizeof(client);
    if (getpeername(fd, (struct sockaddr *)&client, &len)) {
        HttpServer::throwError("getpeername");
    }
        
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
    sprintf(write_str, "closed connection from: address: %s, port: %s\n", host, port);
    write(STDOUT_FILENO, write_str, strlen(write_str));

    close(fd);
}

void ThreadPool::WorkRoutine() {
    while (!need_stop) {
        auto fd = queue_.get();
        if (fd.has_value()) {
            process_client(fd.value());
        }
    }
}

