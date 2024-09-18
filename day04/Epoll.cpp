#include "Epoll.h"
#include "../utils/util.h"
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

std::vector<epoll_event> Epoll::poll(int timeout)
{
    std::vector<epoll_event> active_events;
    // printf("timeout = %d\n", timeout);
    // printf("here epoll wait .......\n");
    int fds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    // printf("here epoll evoke .......\n");
    // printf("here fds %d \n", fds);
    errif(fds == -1, "epoll wait error");
    for (int i = 0; i < fds; ++i) // bugs: for (int i = 0; i < 0; ++i)
    {
        active_events.emplace_back(events[i]);
    }
    return active_events;
}
