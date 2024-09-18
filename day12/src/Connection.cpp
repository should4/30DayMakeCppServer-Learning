#include "Connection.h"

#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Buffer.h"
#include "../utils/util.h"

#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>

Connection::Connection()
{
}
Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock)
{
    channel = new Channel(loop, sock->getFd());
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    channel->setCallBack(cb);
    sock->setnonblock();         // 将客户端 Socket I/O 设置为非阻塞模式
    channel->setenableReading(); // 将客户端 channel 添加到 epoll 树中

    readBuffer = new Buffer(); // 建立读缓冲区
}
Connection::~Connection()
{
    delete channel;
    delete sock;
}

void Connection::Connection::echo(int fd)
{
    char buffer[1024];
    while (true)
    {
        bzero(buffer, sizeof(buffer));
        int readbytes = read(fd, buffer, sizeof(buffer));
        if (readbytes > 0)
        {
            printf("client %d,recv %s\n", fd, buffer);
            readBuffer->append(buffer, readbytes);
        }
        else if (readbytes == -1 && errno == EINTR)
        {
            printf("系统终端正常终端，继续\n");
            continue;
        }
        else if (readbytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            printf("一次性从客户端 %d 读完\n", fd);
            errif(write(fd, readBuffer->c_str(), readBuffer->size()) == -1, "write error");
            readBuffer->clear(); // 发送完成 清空缓冲区
            break;
        }
        else if (readbytes == 0)
        {
            printf("客户端 %d 断开连接\n", fd);
            deleteConnection(); // 客户端断开 调用断开连接函数（即 Server 中的释放连接的回调函数）
            break;
        }
        else
        {
            printf("读取出现了一些问题,来自 %d 客户端\n", fd);
            break;
        }
    }
}

void Connection::deleteConnection()
{
    deleteConnetionCallback(sock);
}

void Connection::setDeleteConnetionCallback(std::function<void(Socket *sock)> cb)
{
    deleteConnetionCallback = cb;
}