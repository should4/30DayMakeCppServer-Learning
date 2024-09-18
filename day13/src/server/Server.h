#pragma once
#include <map>    // 用来管理 所有连接
#include <vector> // 用来管理所有 subReactor

class EventLoop;
class Acceptor;
class ThreadPool;
class Connection;
class Socket;
class Server
{
private:
    Acceptor *acceptor;
    EventLoop *mainReactor;
    std::vector<EventLoop *> subReactors;
    std::map<int, Connection *> connections; // 所有连接
    ThreadPool *thread_pool;

public:
    Server(EventLoop *_loop); // mainReactor所在线程由 主线程给出
    ~Server();

    void newConnetion(Socket *sock);
    void deleteConnection(Socket *sock);
};