#include "Acceptor.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "../log/logger.h"

#include <stdio.h>

Acceptor::Acceptor() : sock(nullptr), channel(nullptr), loop(nullptr)
{
}

Acceptor::Acceptor(EventLoop *_loop) : loop(_loop)
{
    sock = new Socket();
    addr = new InetAddress(ip, port); // InetAddress *addr = new InetAddress(ip, port);
    sock->bind(addr);
    sock->listen();
    sock->setnonblock(); // 将接受连接监听 Socket I/O 设置为非阻塞 模式

    channel = new Channel(loop, sock->getFd());
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this); // 设置 channel 的回调函数
    channel->setCallBack(cb);
    channel->setenableReading(); // 将 channel 添加到 epoll 监听树上
}

Acceptor::~Acceptor()
{
    delete sock;
    delete addr;
    delete channel;
}

void Acceptor::acceptConnection()
{
    InetAddress *clnt_addr = new InetAddress();
    Socket *clnt_sock = new Socket(sock->accept(clnt_addr));
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), clnt_addr->getIp(), clnt_addr->getPort());

    clnt_sock->setnonblock(); // 新接受到的连接设置为非阻塞式
    newConnectionCallback(clnt_sock);
    delete clnt_addr;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket *)> cb)
{
    newConnectionCallback = cb;
}