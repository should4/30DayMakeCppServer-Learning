#include "Acceptor.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"

Acceptor::Acceptor() : sock(nullptr), channel(nullptr), loop(nullptr)
{
}

Acceptor::Acceptor(EventLoop *_loop) : loop(_loop)
{
    sock = new Socket();
    addr = new InetAddress(ip, port); // InetAddress *addr = new InetAddress(ip, port);
    sock->bind(addr);
    sock->listen();
    sock->setnonblock(); // 将接受连接监听 I/O 设置为 ET 模式

    channel = new Channel(loop, sock->getFd());
    std::function<void()> cb = std::bind(&Acceptor::newConnection, this); // 设置 channel 的回调函数
    channel->setCallBack(cb);
    channel->setenableReading(); // 将 channel 添加到 epoll 监听树上
}

Acceptor::~Acceptor()
{
    delete sock;
    delete addr;
    delete channel;
}

void Acceptor::newConnection()
{
    callback(sock);
}

void Acceptor::setCallBack(std::function<void(Socket *)> cb)
{
    callback = cb;
}