#pragma once

class Epoll;
class Channel;
class EventLoop
{
private:
    Epoll *ep;
    bool quit; // 控制事件循环停止

public:
    EventLoop();
    ~EventLoop();

    void loop();

    void updateChannel(Channel *channel);
};