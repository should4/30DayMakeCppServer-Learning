#pragma once

class Epoll;
class Channel;
class EventLoop
{
private:
    Epoll *ep;

public:
    EventLoop();
    ~EventLoop();

    // 更新 epoll 监听树中的 channel
    void updateChannel(Channel *channel);

    // 开始轮训监听 epoll 树
    void loop();
};
