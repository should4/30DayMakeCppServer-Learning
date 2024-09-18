#include "Epoll.h"
#include "util.h"
#include "Channel.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define MAX_EVENTS 1000

Epoll::Epoll() : epfd(-1), events(nullptr)
{
    epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");
    events = new epoll_event[MAX_EVENTS];
    bzero(events, sizeof(events) * MAX_EVENTS);
}

Epoll::~Epoll()
{
    if (epfd != -1)
    {
        close(epfd);
        epfd = -1;
    }
    delete[] events;
}

void Epoll::addFd(int fd, uint32_t op)
{
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.events = op;
    ev.data.fd = fd;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event error");
}

void Epoll::updateChannel(Channel *channel)
{
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.events = channel->getEvents();
    ev.data.ptr = channel;

    printf("here client %d try to update epoll\n", channel->getFd());
    if (!channel->isInEpoll()) // Channel 不在 epoll 树时，将其添加进入
    {
        channel->setInEpoll(true);
        errif(epoll_ctl(epfd, EPOLL_CTL_ADD, channel->getFd(), &ev) == -1, "epoll add error");
    }
    else // Channel 在 epoll 树时，修改希望监听的事件类型
    {
        errif(epoll_ctl(epfd, EPOLL_CTL_MOD, channel->getFd(), &ev) == -1, "epoll modified error");
    }
}

std::vector<Channel *> Epoll::poll(int timeout)
{
    int fds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(fds == -1, "epoll wait error");

    std::vector<Channel *> active_channels;
    for (int i = 0; i < fds; ++i)
    {
        auto temp = (Channel *)events[i].data.ptr;
        temp->setRevents(events[i].events); // 设置当前文件描述符发生的事件
        active_channels.emplace_back(temp);
    }

    return active_channels;
}

// std::vector<epoll_event> Epoll::poll(int timeout)
// {
//     std::vector<epoll_event> active_events;
//     // printf("timeout = %d\n", timeout);
//     // printf("here epoll wait .......\n");
//     int fds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
//     // printf("here epoll evoke .......\n");
//     // printf("here fds %d \n", fds);
//     errif(fds == -1, "epoll wait error");
//     for (int i = 0; i < fds; ++i) // bugs: for (int i = 0; i < 0; ++i)
//     {
//         active_events.emplace_back(events[i]);
//     }
//     return active_events;
// }
