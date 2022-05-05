#include <server/server.h>
#include <utils/helpers.h>
#include <cstdlib>
#include <memory>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " PORT" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::size_t port;
    sscanf(argv[1], "%ld", &port);

    std::cout << "my pid: " << getpid() << std::endl;
    std::cout << "listening at port: " << port << std::endl;

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

    bool need_exit = false;
    while (!need_exit) {
        try {
            need_exit = server->run();
        } catch(const HttpServerError& error) {
            log(RED "ERROR: %s\n" RESET, error.why.c_str());
        } catch(...) {
            log(RED "UNKNOWN ERROR\n" RESET);
        }
    }

    return 0;
}
