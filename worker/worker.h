#pragma once

#include <thread_pool/thread_pool.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <utils/helpers.h>

class ThreadWorker {
private:
    ThreadPool<ThreadWorker>* pool;

    void process_client(std::size_t fd);

public:

    ThreadWorker(ThreadPool<ThreadWorker>* pool);

    void WorkRoutine();

};
