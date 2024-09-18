#include "Channel.h"
#include "Epoll.h"
#include "EventLoop.h"

// Channel::Channel() : ep(nullptr), fd(-1), inEpoll(false)
// {
// }
// Channel::Channel(Epoll *ep, int fd) : ep(ep), fd(fd), inEpoll(false)
// {
// }

Channel::Channel() : loop(nullptr), fd(-1), inEpoll(false)
{
}

Channel::Channel(EventLoop *loop, int fd) : loop(loop), fd(fd)
{
}

Channel::~Channel() {}

void Channel::setenableReading(uint32_t events)
{
    this->events = events; // 默认设置为 ET 模式
    loop->updateChannel(this);
}

int Channel::getFd()
{
    return this->fd;
}

bool Channel::isInEpoll()
{
    return inEpoll;
}

uint32_t Channel::getEvents()
{
    return events;
}
uint32_t Channel::getRevents()
{
    return revents;
}
void Channel::setRevents(uint32_t revents)
{
    this->revents = revents;
}

void Channel::setInEpoll(bool flag)
{
    this->inEpoll = flag;
}

void Channel::setCallBack(std::function<void()> cb)
{
    this->call_back = cb;
}

void Channel::handleEvent()
{
    // 当前只处理 可读事件 故只有一个回调函数
    call_back();
}