#pragma once

#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <queue>

struct ThreadWork
{
    void (*function)(void*) = nullptr;
    void* arg = nullptr;

    ThreadWork() {}
    ThreadWork(void (*function)(void* functionArg), void* arg)
        : function(function), arg(arg) {}
};

struct ThreadPool
{
    int workingCount = 0;
    int threadCount = 0;

    std::queue<ThreadWork> workQueue = std::queue<ThreadWork>();

    std::condition_variable workCond;
    std::condition_variable finishedCond;
    std::mutex mutex;
    bool stop = false;

    void addWork(void (*function)(void* arg), void* arg);
    void destroy();
    void wait();
    void init(int threadCount);
};

inline ThreadPool globalThreadPool;
