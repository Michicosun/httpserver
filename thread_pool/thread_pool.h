#pragma once

#include <vector>
#include <blocking_queue/blocking_queue.h>


template<class Value, class Worker>
class ThreadPool{
private:
    std::vector<std::thread> threads_;
    BlockingQueue<Value> queue_;
    bool need_stop = false;

public:

    ThreadPool(std::size_t thread_count) : queue_(std::chrono::seconds(1)) {
        for (int i = 0; i < thread_count; ++i) {
            threads_.emplace_back([this] () {
                Worker worker(this);
                worker.WorkRoutine();
            });
        }
    }

    bool needStop() const {
        return need_stop;
    }

    std::optional<Value> get() {
        return queue_.get();
    }

    void submit(const Value& fd) {
        queue_.push(fd);
    }

    void join() {
        need_stop = true;
        for (auto& el : threads_) {
            el.join();
        }
        threads_.clear();
    }

};