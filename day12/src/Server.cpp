#include "Server.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "ThreadPool.h"
#include "Socket.h"
#include "Connection.h"
#include "InetAddress.h"

Server::Server(EventLoop *_loop) : acceptor(nullptr), mainReactor(_loop), thread_pool(nullptr)
{
    acceptor = new Acceptor(mainReactor);
    std::function<void(Socket *)> cb = std::bind(&Server::newConnetion, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);

    int size = std::thread::hardware_concurrency(); // 线程池中国线程数
    thread_pool = new ThreadPool(size);
    // 每个 subReactor 对应一个 EventLoop
    for (int i = 0; i < size; ++i)
    {
        subReactors.emplace_back(new EventLoop());
    }
    // one loop per one thread
    for (int i = 0; i < size; ++i)
    {
        std::function<void()> task = std::bind(&EventLoop::loop, subReactors[i]);
        thread_pool->add(task);
    }
}
Server::~Server()
{
}

void Server::newConnetion(Socket *sock)
{
    if (sock->getFd() != -1)
    {
        // //调度策略：全随机
        int random = sock->getFd() % subReactors.size();
        Connection *connection = new Connection(subReactors[random], sock);
        std::function<void(Socket *)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
        connection->setDeleteConnetionCallback(cb);     // 设置 TCP 连接的释放回调函数（当客户端断开时调用）
        connections[sock->getFd()] = connection; // 将新的 TCP 连接放入 map 中
    }
}
void Server::deleteConnection(Socket *sock)
{
    int fd = sock->getFd();
    Connection *conn = connections[fd];
    connections.erase(fd);
    delete conn;
}