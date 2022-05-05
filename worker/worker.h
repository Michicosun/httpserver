#pragma once

#include <thread_pool/thread_pool.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <utils/helpers.h>
#include <cstddef>

class ThreadWorker {
private:
    ThreadPool<std::size_t, ThreadWorker>* pool;

    void process_client(std::size_t fd);

public:

    ThreadWorker(ThreadPool<std::size_t, ThreadWorker>* pool);

    void WorkRoutine();

};
