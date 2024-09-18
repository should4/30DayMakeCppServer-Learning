#pragma once

class Socket;
class EventLoop;
class Server
{
private:
    EventLoop *loop;

public:
    Server();
    Server(EventLoop *loop, char *ip = "127.0.0.1", int port = 8888);
    ~Server();

    // 接受新的连接（把新客户端加到 epoll 树上） 回调函数
    void newConnection(Socket *serv_sock);

    // 客户端连接 处理可读事件
    void handleReadEvent(int fd);
};
