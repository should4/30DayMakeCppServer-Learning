#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include "ThreadPool.h"

EventLoop::EventLoop()
{
    ep = new Epoll(); // 建立 epoll 树
    pool = new ThreadPool(std::thread::hardware_concurrency());
}

EventLoop::~EventLoop()
{
    delete pool;
    delete ep;
}

void EventLoop::updateChannel(Channel *channel)
{
    ep->updateChannel(channel);
}

void EventLoop::loop()
{
    while (true)
    {
        std::vector<Channel *> channels;
        channels = ep->poll();
        for (auto itr = channels.begin(); itr != channels.end(); ++itr)
        {
            (*itr)->handleEvent();
        }
    }
}

void EventLoop::addThread(std::function<void()> func)
{
    pool->add(func);
}