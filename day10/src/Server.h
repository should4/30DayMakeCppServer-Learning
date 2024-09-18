#pragma once
#include <map>

class EventLoop;
class Acceptor;
class Connection;
class Socket;
class Server
{
private:
    EventLoop *loop;
    Acceptor *acceptor;
    std::map<int, Connection *> connections; // 用来存储 TCP客户端连接

public:
    Server();
    Server(EventLoop *_loop);
    ~Server();

    /**
     * @brief Acceptor 的回调函数
     */
    void newConnection(Socket *sock);

    /**
     * @brief Connetion 的回调释放函数
     * @param sock : 疑问？为什么不直接返回 Connetion *
     */
    void deleteConnetion(Socket *sock);
};