#pragma once
#include <sys/epoll.h>
#include <functional>

class Epoll;
class EventLoop;
class Channel
{
private:
    // Epoll *ep;        // 对应的 epoll树
    EventLoop *loop;  // 对应 reactor loop
    int fd;           // 文件描述符
    uint32_t events;  // 表示希望监听这个文件描述符的哪些时间
    uint32_t revents; // 表示当前文件描述符发生的事件
    bool inEpoll;     // 表示当前 Channel 是否已经在 epoll 红黑树中

    std::function<void()> call_back; // 可读事件处理的回调函数

public:
    Channel();
    // Channel(Epoll *ep, int fd);
    Channel(EventLoop *loop, int fd);
    ~Channel();

    void setenableReading(uint32_t events = EPOLLIN | EPOLLET); // 将当前 Channel 添加到 epoll 树中
    bool isInEpoll();                                           // 返回 inEpoll 的值，表示当前 Channel 是否已在 epoll 树中

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    void setRevents(uint32_t revents);
    void setInEpoll(bool flag);

    // 设置回调函数
    void setCallBack(std::function<void()> cb);

    // 事件处理函数
    void handleEvent();
};
