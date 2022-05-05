#include <utils/helpers.h>

void throwError(std::string pref) {
    char* err = std::strerror(errno);
    std::string serr(err);
    throw HttpServerError{pref + ": " + serr};
} 

