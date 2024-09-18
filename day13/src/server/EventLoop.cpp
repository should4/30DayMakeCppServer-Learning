#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"

EventLoop::EventLoop() : ep(nullptr), quit(false)
{
    ep = new Epoll();
}
EventLoop::~EventLoop()
{
    delete ep;
}

void EventLoop::loop()
{
    // 此处可能存在 bug 因外侧无法控制 quit 的值而使循环无法退出
    while (!quit)
    {
        std::vector<Channel *> channels = ep->poll();
        for (auto itr = channels.begin(); itr != channels.end(); ++itr)
        {
            (*itr)->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel *channel)
{
    ep->updateChannel(channel);
}