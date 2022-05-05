#include <utils/helpers.h>

void throwError(std::string pref) {
    char* err = std::strerror(errno);
    std::string serr(err);
    throw HttpServerError{pref + ": " + serr};
} 

void log(const char* format_s) {
    write(STDOUT_FILENO, format_s, strlen(format_s));
}



