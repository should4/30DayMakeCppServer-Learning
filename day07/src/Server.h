#pragma once

class Socket;
class EventLoop;
class Acceptor;
class Server
{
private:
    EventLoop *loop;
    Acceptor *acceptor;

public:
    Server();
    Server(EventLoop *loop);
    ~Server();

    // 接受新的连接（把新客户端加到 epoll 树上） 回调函数
    void newConnection(Socket *serv_sock);

    // 客户端连接 处理可读事件
    void handleReadEvent(int fd);
};
