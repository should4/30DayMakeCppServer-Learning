#pragma once
#include <sys/epoll.h>

class Epoll;
class Channel
{
private:
    Epoll *ep;        // 对应的 epoll树
    int fd;           // 文件描述符
    uint32_t events;  // 表示希望监听这个文件描述符的哪些时间
    uint32_t revents; // 表示当前文件描述符发生的事件
    bool inEpoll;     // 表示当前 Channel 是否已经在 epoll 红黑树中

public:
    Channel();
    Channel(Epoll *ep, int fd);
    ~Channel();

    void setenableReading(uint32_t eventas = EPOLLIN | EPOLLET); // 将当前 Channel 添加到 epoll 树中
    bool isInEpoll();                                            // 返回 inEpoll 的值，表示当前 Channel 是否已在 epoll 树中

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    void setRevents(uint32_t revents);
    void setInEpoll(bool flag);
};
