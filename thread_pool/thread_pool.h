#pragma once

#include <cstddef>
#include <vector>
#include <blocking_queue/blocking_queue.h>

class ThreadPool{
private:

    void WorkRoutine();
    std::vector<std::thread> threads_;
    BlockingQueue<std::size_t> queue_;
    bool need_stop = false;

public:

    ThreadPool(std::size_t thread_count);

    void submit(std::size_t fd);

    void join();
};