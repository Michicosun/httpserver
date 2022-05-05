#include <sys/socket.h>
#include <unistd.h>
#include <worker/worker.h>
#include <thread_pool/thread_pool.h>
#include <utils/helpers.h>

#include <cstring>
#include <iostream>

extern bool long_log;

ThreadWorker::ThreadWorker(ThreadPool<std::size_t, ThreadWorker>* pool) : pool{pool} {}

void ThreadWorker::process_client(std::size_t connection) {
    HttpRequest request = parseRequest(connection);

    log("[" MAG "%d" RESET "] -> connection: %ld, request: "
        "{" 
        GRN "TYPE: " RESET "%s, "
        GRN "PATH: " RESET "%s, "
        GRN "PROTO: " RESET "%s}\n"
        , getpid()
        , connection
        , request.rtype.c_str()
        , request.path.c_str()
        , request.proto.c_str());
    
    if (long_log) {
        for (const auto& [k, v] : request.headers) {
            log(
                "[" MAG "%d" RESET "] " 
                GRN "KEY: " RESET "%s " 
                CYN "VALUE: " RESET "%s\n"
                , getpid()
                , k.c_str()
                , v.c_str());
        }

        log(
            "[" MAG "%d" RESET "] " 
            GRN "BODY: " RESET "%s\n"
            , getpid()
            , request.body.c_str());
    }

    sendResponse(connection, request);

    struct sockaddr_in client = {};
    socklen_t len = sizeof(client);
    if (getpeername(connection, (struct sockaddr *)&client, &len) < 0) {
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

    close(connection);
}

void ThreadWorker::WorkRoutine() {
    while (!pool->needStop()) {
        try {
            auto connection = pool->get();
            if (connection.has_value()) {
                process_client(connection.value());
            }
        } catch(const HttpServerError& error) {
            log(
                "[" MAG "%d" RESET "] "
                RED "ERROR: %s\n" RESET
                , getpid()
                , error.why.c_str());
        } catch(...) {
            log(
                "[" MAG "%d" RESET "] "
                RED "UNKNOWN ERROR\n" RESET
                , getpid());
        }
    }
}
