#include <unistd.h>
#include <worker/worker.h>
#include <thread_pool/thread_pool.h>
#include <utils/helpers.h>

#include <iostream>

ThreadWorker::ThreadWorker(ThreadPool<ThreadWorker>* pool) : pool{pool} {}


void ThreadWorker::process_client(std::size_t fd) {
    log(CYN "worker" RESET " %d get: %ld\n", getpid(), fd);

    char c;
    while (read(fd, &c, 1) == 1 && c != EOF) {
        write(STDOUT_FILENO, &c, 1);
    }
        
    struct sockaddr_in client = {};
    socklen_t len = sizeof(client);
    if (getpeername(fd, (struct sockaddr *)&client, &len)) {
        throwError("getpeername");
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

    log(RED "closed connection" RESET " from: address: %s, port: %s\n", host, port);

    close(fd);
}

void ThreadWorker::WorkRoutine() {
    while (!pool->needStop()) {
        auto fd = pool->get();
        if (fd.has_value()) {
            process_client(fd.value());
        }
    }
}
