#pragma once

#include <sstream>
#include <unistd.h>

class Logger {
private:
    std::stringstream buffer_;

    struct flusher {};

public:

    inline static flusher endl;

    template<class T>
    Logger& operator<< (const T& el) {
        buffer_ << el;
        return *this;
    }

    Logger& operator<< (const flusher& flusher) {
        buffer_ << '\n';
        std::size_t cnt = 0;
        std::size_t buf_len = buffer_.str().size();
        while (cnt < buf_len) {
            cnt += write(STDOUT_FILENO, buffer_.str().c_str() + cnt, buf_len - cnt);
        }
        buffer_.str("");
        return *this;
    }

};
