#include "threadPool.hpp"
#include <thread>
#include "assertm.hpp"

void workerLoop(void* arg)
{
    ThreadPool& threadPool = *(ThreadPool*)arg;

    while (true) {
        ThreadWork work;
        {
            std::unique_lock<std::mutex> lock(threadPool.mutex);

            while (threadPool.workQueue.size() == 0 && !threadPool.stop) {
                threadPool.workCond.wait(lock);
            }

            if (threadPool.stop) {
                threadPool.threadCount -= 1;
                threadPool.finishedCond.notify_all();
                return;
            }

            if (threadPool.workQueue.size() == 0) {
                continue;
            }

            work = threadPool.workQueue.front();
            threadPool.workQueue.pop();
            threadPool.workingCount += 1;
        }

        assertm(work.function != nullptr, "function pointer passed to thread is null");
        work.function(work.arg);

        {
            std::unique_lock<std::mutex> lock(threadPool.mutex);

            threadPool.workingCount -= 1;
            if (!threadPool.stop && threadPool.workingCount == 0 && threadPool.workQueue.size() == 0) {
                threadPool.finishedCond.notify_all();
            }
        }
    }
}

void ThreadPool::init(int size)
{
    assertm(size != 0, "Creating thread pool with 0 threads");

    threadCount = size;

    for (int i = 0; i < threadCount; i++) {
        std::thread thread(workerLoop, this);
        thread.detach();
    }
}

void ThreadPool::addWork(void (*function)(void*), void* arg)
{
    std::unique_lock<std::mutex> lock(mutex);

    assertm(function != nullptr, "function added as work is null");

    ThreadWork threadWork(function, arg);
    workQueue.push(threadWork);

    workCond.notify_all();
}

void ThreadPool::destroy()
{
    {
        std::unique_lock<std::mutex> lock(mutex);
        while (workQueue.size() > 0) {
            workQueue.pop();
        }
        stop = true;
        workCond.notify_all();
    }

    wait();
}

void ThreadPool::wait()
{
    std::unique_lock<std::mutex> lock(mutex);

    while (true) {
        if (workQueue.size() != 0 // have work left
            || (!stop && workingCount > 0) // threads are processing
            || (stop && threadCount != 0)) // threads are still exiting
        {
            finishedCond.wait(lock);
        }
        else {
            break;
        }
    }
}

