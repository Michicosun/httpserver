#include <server/server.h>
#include <unistd.h>
#include <utils/helpers.h>
#include <cstdlib>
#include <cstring>
#include <memory>

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

    log(GRN "server pid: " RESET "%d\n", getpid());
    log(GRN "listening at port: " RESET "%d\n", port);

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
        log(RED "ERROR: %s\n" RESET, error.why.c_str());
        exit(EXIT_FAILURE);
    } catch(...) {
        log(RED "UNKNOWN ERROR\n" RESET);
        exit(EXIT_FAILURE);
    }

    try {
        server->run();
    } catch(const HttpServerError& error) {
        log(RED "ERROR: %s\n" RESET, error.why.c_str());
    } catch(...) {
        log(RED "UNKNOWN ERROR\n" RESET);
    }

    server->shutdown();

    return 0;
}
