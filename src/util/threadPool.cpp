#include "threadPool.hpp"

#include <pthread.h>

#include "assertm.hpp"

void* workerLoop(void* arg)
{
    ThreadPool& threadPool = *(ThreadPool*)arg;

    while (true) {
        pthread_mutex_lock(&threadPool.mutex);
        
        while (threadPool.workQueue.size() == 0 && !threadPool.stop) {
            pthread_cond_wait(&threadPool.workCond, &threadPool.mutex);
        }

        if (threadPool.stop) {
            threadPool.threadCount -= 1;
            pthread_cond_signal(&threadPool.finishedCond);
            pthread_mutex_unlock(&threadPool.mutex);
            return nullptr;
        }


        if (threadPool.workQueue.size() != 0) {

            ThreadWork work = threadPool.workQueue.front();
            threadPool.workQueue.pop();
            threadPool.workingCount += 1;

            assertm(work.function != nullptr, "function pointer passed to thread is null");
            //assertm(work.arg != nullptr, "arg can be null and is null but testing");

            pthread_mutex_unlock(&threadPool.mutex);

            work.function(work.arg);

            pthread_mutex_lock(&threadPool.mutex);

            threadPool.workingCount -= 1;
            if (!threadPool.stop && threadPool.workingCount == 0 && threadPool.workQueue.size() == 0) {
                pthread_cond_signal(&threadPool.finishedCond);
            }
        }
        pthread_mutex_unlock(&threadPool.mutex);
    }
    return nullptr;
}

void ThreadPool::init(int size)
{
    assertm(size != 0, "Creating thread pool with 0 threads");

    threadCount = size;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&workCond, NULL);
    pthread_cond_init(&finishedCond, NULL);

    for (int i = 0; i < threadCount; i++) {
        pthread_t thread;
        int result = pthread_create(&thread, NULL, &workerLoop, this);
        assertm(result == 0, "error creating thread");
        pthread_detach(thread);
    }
}

void ThreadPool::addWork(void (*function)(void*), void* arg)
{
    pthread_mutex_lock(&mutex);

    assertm(function != nullptr, "function added as work is null");
    //assertm(arg != nullptr, "arg added is null");

    ThreadWork threadWork{function, arg};
    workQueue.push(threadWork);

    pthread_cond_broadcast(&workCond);
    pthread_mutex_unlock(&mutex);
}

void ThreadPool::destroy()
{
    wait();

    pthread_mutex_lock(&mutex);
    while (workQueue.size() > 0) {
        workQueue.pop();
    }
    stop = true;
    pthread_cond_broadcast(&workCond);
    pthread_mutex_unlock(&mutex);

    wait();

    pthread_mutex_lock(&mutex);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&workCond);
    pthread_cond_destroy(&finishedCond);

    pthread_mutex_unlock(&mutex);
}

void ThreadPool::wait()
{
    pthread_mutex_lock(&mutex);

    while (true) {
        if (workQueue.size() != 0 // have work left
            || (!stop && workingCount > 0) // threads are processing
            || (stop && threadCount != 0)) // threads are still exiting
        {
            pthread_cond_wait(&finishedCond, &mutex);
        }
        else {
            break;
        }
    }

    pthread_mutex_unlock(&mutex);
}

