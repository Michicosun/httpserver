#pragma once

#include <thread_pool/thread_pool.h>
#include <worker/worker.h>

#include <cstddef>
#include <string>
#include <csignal>


class HttpServer {
private:
    void create_listener(std::size_t port);

    std::size_t accept_connection();

private:

    ThreadPool<ThreadWorker> pool;    

    size_t backlog;
    size_t port;
    size_t lfd;

    sigset_t mask;
    size_t signal_fd;

public:

    HttpServer(size_t port, sigset_t mask, size_t backlog = 5);

    bool run();

    void shutdown();
};


