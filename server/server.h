#pragma once

#include <thread_pool/thread_pool.h>
#include <worker/worker.h>
#include <utils/logger.h>

#include <csignal>


class HttpServer {
private:
    void create_listener(std::size_t port);

    int accept_connection();

private:

    ThreadPool<int, ThreadWorker> pool;    

    std::size_t backlog;
    std::size_t port;
    int lfd;

    sigset_t mask;
    int signal_fd;

    Logger log;

public:

    HttpServer(std::size_t port, sigset_t mask, std::size_t backlog = 5);

    bool run();

    void shutdown();
};


