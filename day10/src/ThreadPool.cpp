#include "ThreadPool.h"

ThreadPool::ThreadPool(int size) : stop(false)
{
    // 初始化线程池
    for (int i = 0; i < size; ++i)
    {
        threads.emplace_back(std::thread([this]() { // 定义每个线程的工作函数
            while (true)
            {
                std::function<void()> task;
                {
                    // 定义互斥锁
                    std::unique_lock<std::mutex> lock(tasks_mtx);
                    cv.wait(lock, [this]()
                            { return stop || !tasks.empty(); });

                    // 如果线程池正处于关闭状态 且 任务队列已经消耗完成 则直接退出
                    if (stop && tasks.empty())
                    {
                        return;
                    }

                    // 从任务队列中拿出一个任务
                    task = tasks.front();
                    tasks.pop();
                }
                task();
            }
        }));
    }
}
ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        stop = true;
    }
    cv.notify_all(); // 让当前已经被挂起的线程退出死循环，结束处理任务的使命
    // 让正在处理任务的线程完成正在处理的任务之后退出，结束处理任务的使命
    for (std::thread &th : threads)
    {
        if (th.joinable())
            th.join();
    }
}

// 向任务队列中添加任务
void ThreadPool::add(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        // 当线程池正在处于关闭状态时，将无法添加新的任务
        if (stop)
        {
            throw std::runtime_error("thread poll has stopped,cannot add task");
        }
        tasks.emplace(task);
    }
    cv.notify_one(); // 通知一个等待任务的线程
}
