#pragma once

#include <pthread.h>
#include <queue>

struct ThreadWork
{
    void (*function)(void*) = nullptr;
    void* arg = nullptr;
};

struct ThreadPool
{
    int workingCount = 0;
    int threadCount = 0;

    std::queue<ThreadWork> workQueue = std::queue<ThreadWork>();

    pthread_cond_t workCond;
    pthread_cond_t finishedCond;
    pthread_mutex_t mutex;
    bool stop = false;

    void addWork(void (*function)(void* arg), void* arg);
    void destroy();
    void wait();
    void init(int threadCount);
};

inline ThreadPool globalThreadPool;

