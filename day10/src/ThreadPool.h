#pragma once
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool
{
private:
    std::vector<std::thread> threads;        // 线程池
    std::queue<std::function<void()>> tasks; // 任务队列
    std::mutex tasks_mtx;                    // 任务队列锁
    std::condition_variable cv;              // 条件变量 用来实现当任务队列中没有任务时 cpu 不轮询线程池中的各个线程，将线程挂起处于休眠状态，等到有任务时再唤醒一个线程

    bool stop;

public:
    ThreadPool(int size);
    ~ThreadPool();

    // 向任务队列中添加任务
    void add(std::function<void()> task);
};