#pragma once
#include <functional>

class Epoll;
class Channel;
class ThreadPool;
class EventLoop
{
private:
    Epoll *ep;
    ThreadPool *pool; // 线程池

public:
    EventLoop();
    ~EventLoop();

    // 更新 epoll 监听树中的 channel
    void updateChannel(Channel *channel);

    // 开始轮训监听 epoll 树
    void loop();

    // 添加任务到任务队列
    void addThread(std::function<void()> func);
};
