#pragma once
#include <functional>

class Socket;
class Channel;
class EventLoop;
class InetAddress;
class Acceptor
{
private:
    Socket *sock;
    EventLoop *loop;
    Channel *channel;
    InetAddress *addr;

    // 服务器监听 ip 与 port
    const char *ip = "127.0.0.1";
    int port = 8888;

public:
    Acceptor();
    Acceptor(EventLoop *_loop);
    ~Acceptor();

    std::function<void(Socket *)> newConnectionCallback; // Acceptor 的回调函数 将连接的逻辑交给 Server 类处理

    void acceptConnection(); // channel 的回调函数

    void setNewConnectionCallback(std::function<void(Socket *)> cb); // 供 Server 类绑定接受客户端连接的回调函数
};