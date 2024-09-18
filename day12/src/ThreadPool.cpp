#include "ThreadPool.h"

ThreadPool::ThreadPool(int _size) : size(_size), stop(false)
{
    for (int i = 0; i < size; ++i)
    {
        /*pool.emplace_back(std::thread([this]()
                                      {
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
            } }));*/

        pool.emplace_back(std::thread([this]() { // 定义每个线程的工作函数
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

    /*
    std::thread([this](){
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(tasks_mtx);
            cv.wait(lock,[this](){
                return stop || !tasks.empty();
            });

            // 如果线程池正处于关闭状态 且 任务队列已经消耗完成 则直接退出
            if (stop && tasks.empty())
                return;

            task = tasks.front();
            tasks.pop();
        }
        task(); }));
    */
}
ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        stop = true;
    }
    cv.notify_all();

    for (auto itr = pool.begin(); itr != pool.end(); ++itr)
    {
        if ((*itr).joinable())
            (*itr).join();
    }
}

void ThreadPool::add(std::function<void()> task) // 向线程池中添加
{
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        // 当线程池正在处于关闭状态时，将无法添加新的任务
        if (stop)
        {
            throw std::runtime_error("thread poll has stopped,cannot add task");
        }
        // tasks.push(task);
        tasks.emplace(task);
    }
    cv.notify_one();
}