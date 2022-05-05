#pragma once

#include <thread_pool/thread_pool.h>

#include <cstddef>
#include <string>


class HttpServer {
private:
    struct HttpServerError {
        std::string why;
    };

    void create_listener(std::size_t port);

    std::size_t accept_connection();

private:

    ThreadPool pool;    

    size_t backlog;
    size_t port;
    size_t lfd;

public:

    HttpServer(size_t port, size_t backlog = 5);

    void run();

    [[ noreturn ]] static void throwError(std::string prefix);


    // void shutdown();

    // void signal_handler(int sig);
};


