#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <condition_variable>
#include <mutex>

class ThreadPool
{
private:
    std::vector<std::thread> pool;         // 线程池
    std::queue<std::function<void()>> tasks; // 任务队列
    std::condition_variable cv;             // 条件变量
    std::mutex tasks_mtx;                         // 锁
    bool stop;                               // 用来关闭线程池
    int size;                                // 线程池大小

public:
    ThreadPool(int _size);
    ~ThreadPool();

    void add(std::function<void()> task); // 向线程池中添加
};