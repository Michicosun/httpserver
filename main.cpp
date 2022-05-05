#include <server/server.h>
#include <unistd.h>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " PORT" << std::endl;
        exit(EXIT_FAILURE);
    }
  

    std::size_t port;
    sscanf(argv[1], "%ld", &port);

    std::cout << "my pid: " << getpid() << std::endl;
    std::cout << "listening at port: " << port << std::endl;

    HttpServer server(port);

    server.run();

    return 0;
}
