#pragma once
#include <sys/epoll.h>
#include <vector>

class Channel; // very important
class Epoll
{
private:
    int epfd;
    struct epoll_event *events;

public:
    Epoll();
    ~Epoll();

    void addFd(int fd, uint32_t op);
    void updateChannel(Channel *channel); // 将 Channel 添加到 epoll  树中

    std::vector<Channel *> poll(int timeout = -1);
    // std::vector<epoll_event> poll(int timeout = -1);
};