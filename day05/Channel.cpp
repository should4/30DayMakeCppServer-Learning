#include "Channel.h"
#include "Epoll.h"

Channel::Channel() : ep(nullptr), fd(-1), inEpoll(false)
{
}
Channel::Channel(Epoll *ep, int fd) : ep(ep), fd(fd), inEpoll(false)
{
}

Channel::~Channel() {}

void Channel::setenableReading(uint32_t events)
{
    this->events = events; // 默认设置为 ET 模式
    ep->updateChannel(this);
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
