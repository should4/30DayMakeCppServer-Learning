#pragma once
#include <functional>

class EventLoop;
class Socket;
class Channel;
class Connection
{
private:
    EventLoop *loop;
    Socket *sock;
    Channel *channel;

    // 连接关闭回调函数（传参：TCP连接的客户端 Socket 对象）
    // 交给 Server 类处理，在该类外完成对该实例的析构与资源回收
    std::function<void(Socket *sock)> deleteConnetionCallback;

public:
    Connection();
    Connection(EventLoop *_loop, Socket *_sock);
    ~Connection();

    // 事件处理函数
    void echo(int fd);

    // Connection 释放回收函数（当客户端断开时，自动关闭）
    void deleteConnection();
    void setDeleteConnetionCallback(std::function<void(Socket *sock)> cb);
};