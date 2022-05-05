#pragma once

#include <chrono>
#include <cstddef>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <optional>

template <class Value>
class BlockingQueue {
private:
    std::chrono::duration<std::size_t> timeout;

    std::deque<Value> buffer_;
    std::mutex mutex;
    std::condition_variable not_empty;

    std::optional<Value> extractElement() {
        if (buffer_.empty()) {
            return {};
        }
        Value el(std::move(buffer_.front()));
        buffer_.pop_front();
        return el;
    }

public:
    BlockingQueue(std::chrono::duration<std::size_t> timeout) : timeout{timeout} {}

    template<class T>
    void push(T&& el) {
        std::lock_guard lock(mutex);
        buffer_.push_back(std::forward<T>(el));
        not_empty.notify_one();
    }

    std::optional<Value> get() {
        std::unique_lock lock(mutex);
        while (buffer_.empty()) {
            not_empty.wait_for(lock, timeout);
        }
        return extractElement();
    }
};





