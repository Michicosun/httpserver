#pragma once

#include <thread_pool/thread_pool.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <utils/helpers.h>
#include <cstddef>
#include <utils/logger.h>

class ThreadWorker {
private:
    ThreadPool<int, ThreadWorker>* pool;

    void process_client(int fd);

    std::string getId();

    Logger log;

public:

    ThreadWorker(ThreadPool<int, ThreadWorker>* pool);

    void WorkRoutine();

};
