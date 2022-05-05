#include <server/server.h>
#include <unistd.h>
#include <utils/helpers.h>
#include <cstdlib>
#include <cstring>
#include <memory>
#include "utils/logger.h"

bool long_log = false;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " PORT" << std::endl;
        exit(EXIT_FAILURE);
    }

    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "-e") == 0) long_log = 1;
        else {
            std::cerr << "Unknown parameter: " << argv[i] << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    std::size_t port;
    sscanf(argv[1], "%ld", &port);

    Logger log;

    log << GRN "server pid: " RESET << getpid() << Logger::endl;
    log << GRN "listening at port: " RESET << port << Logger::endl;

    std::unique_ptr<HttpServer> server;    

    try {
        sigset_t mask = {0};
        sigemptyset(&mask);
        sigaddset(&mask, SIGTERM);
        sigaddset(&mask, SIGQUIT);
        sigaddset(&mask, SIGINT);

        if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
            throwError("sigprocmask");
        }

        server = std::make_unique<HttpServer>(port, mask);
    } catch(const HttpServerError& error) {
        log << RED "ERROR: " << error.why << RESET << Logger::endl;
        exit(EXIT_FAILURE);
    } catch(...) {
        log << RED "UNKNOWN ERROR" RESET << Logger::endl;
        exit(EXIT_FAILURE);
    }

    try {
        server->run();
    } catch(const HttpServerError& error) {
        log << RED "ERROR: " << error.why << RESET << Logger::endl;
    } catch(...) {
        log << RED "UNKNOWN ERROR" RESET << Logger::endl;
    }

    server->shutdown();

    return 0;
}
