#include <worker/worker.h>
#include <thread_pool/thread_pool.h>
#include <utils/helpers.h>
#include <utils/logger.h>

#include <thread>

#include <sys/socket.h>

extern bool long_log;

ThreadWorker::ThreadWorker(ThreadPool<std::size_t, ThreadWorker>* pool) : pool{pool} {}

std::string ThreadWorker::getId() {
    std::size_t id = std::hash<std::thread::id>{}(std::this_thread::get_id());
    std::string sid = std::to_string(id);
    if (sid.size() <= 5) {
        return sid;
    }
    return sid.substr(0, 5);
}

void ThreadWorker::process_client(std::size_t connection) {
    HttpRequest request = parseRequest(connection);

    
    log << "[" << MAG << getId() << RESET 
        << "] -> connection: " << connection 
        << ", request: " << "{" 
        << GRN "TYPE: " RESET << request.rtype << ", "
        << GRN "PATH: " RESET << request.path << ", "
        << GRN "PROTO: " RESET << request.proto << "}" << Logger::endl;
    
    if (long_log) {
        for (const auto& [k, v] : request.headers) {
            log << "[" MAG << getId() << RESET "] " 
                << GRN "KEY: " RESET << k << " " 
                << CYN "VALUE: " RESET << v << Logger::endl;
        }

        log << "[" MAG << getId() << RESET "] " 
            << GRN "BODY: " RESET << request.body << Logger::endl;
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

    log << RED "closed connection" RESET 
        << " from: address: " << host <<", port: " << port << Logger::endl;

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
            log << "[" MAG << getId() << RESET "] " 
                << RED "ERROR: " << error.why << RESET << Logger::endl;
        } catch(...) {
            log << "[" MAG << getId() << RESET "] " 
                << RED "UNKNOWN ERROR" RESET << Logger::endl;
        }
    }
}
