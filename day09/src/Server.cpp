#include "Server.h"

#include "Acceptor.h"
#include "Connection.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"

#include <functional>
#include <stdio.h>

Server::Server()
{
}
Server::Server(EventLoop *_loop) : loop(_loop)
{
    acceptor = new Acceptor(loop);
    std::function<void(Socket *)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);
}
Server::~Server()
{
}

void Server::newConnection(Socket *serv_sock)
{
    InetAddress *client_addr = new InetAddress(); // 没有 delete 有内存泄漏的风险
    Socket *client_sock = new Socket(serv_sock->accept(client_addr));

    Connection *connection = new Connection(loop, client_sock);
    std::function<void(Socket *)> cb = std::bind(&Server::deleteConnetion, this, std::placeholders::_1);
    connection->setDeleteConnetionCallback(cb);     // 设置 TCP 连接的释放回调函数（当客户端断开时调用）
    connections[client_sock->getFd()] = connection; // 将新的 TCP 连接放入 map 中

    // 输出新连接的信息
    printf("当前客户端连接数为：%ld\n", connections.size());
    printf("new client %d,ip %s,port %d \n", client_sock->getFd(), inet_ntoa(client_addr->sock_addr.sin_addr), ntohs(client_addr->sock_addr.sin_port));
}

void Server::deleteConnetion(Socket *sock)
{
    Connection *connection = connections[sock->getFd()];
    connections.erase(sock->getFd());
    delete connection;
}
