#include "Server.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"

#include <stdio.h>
#include <functional>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 1024

Server::Server() : loop(nullptr)
{
}

Server::Server(EventLoop *loop, char *ip, int port) : loop(loop)
{
    Socket *serv_sock = new Socket(); // 未 delete 存在内存泄漏
    InetAddress sock_addr(ip, port);
    serv_sock->bind(&sock_addr);
    serv_sock->listen();

    Channel *channel = new Channel(loop, serv_sock->getFd());
    // 设置服务器新建连接的回调函数
    std::function<void()> cb = std::bind(&Server::newConnection, this, serv_sock);
    channel->setCallBack(cb);
    // 将新接受的客户端 channel 加入 epoll 监听树中
    channel->setenableReading();
}

Server::~Server()
{
}

void Server::newConnection(Socket *serv_sock)
{
    InetAddress *client_addr = new InetAddress();   // BUG : InetAddress client_addr;
    int client_fd = serv_sock->accept(client_addr); // accept(serv_sock->getFd(), (struct sockaddr *)&client_addr.sock_addr, &client_addr.sock_len);
    Socket *client_sock = new Socket(client_fd);
    client_sock->setnonblock(); // 将客户端设置为 非阻塞 I/O（接收发送数据）

    Channel *channel = new Channel(loop, client_fd);
    std::function<void()> cb = std::bind(&Server::handleReadEvent, this, client_fd);
    channel->setCallBack(cb);    // 设置客户端连接回调函数
    channel->setenableReading(); // 将 channel 添加到 epoll 监听树上 默认为 ET 模式

    printf("new client %d,ip %s,port %d \n", client_fd, inet_ntoa(client_addr->sock_addr.sin_addr), ntohs(client_addr->sock_addr.sin_port));
}

void Server::handleReadEvent(int fd)
{
    char buffer[BUFFER_SIZE];
    while (true)
    {
        bzero(buffer, BUFFER_SIZE);
        int readbytes = read(fd, buffer, BUFFER_SIZE);
        if (readbytes > 0)
        {
            printf("client %d,recv %s\n", fd, buffer);
            write(fd, buffer, BUFFER_SIZE);
        }
        else if (readbytes == -1 && errno == EINTR)
        {
            printf("系统终端正常终端，继续\n");
            continue;
        }
        else if (readbytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            printf("一次性从客户端 %d 读完\n", fd);
            break;
        }
        else if (readbytes == 0)
        {
            printf("客户端 %d 断开连接\n", fd);
            close(fd); // 关闭客户端
            break;
        }
        else
        {
            printf("读取出现了一些问题,来自 %d 客户端\n", fd);
            break;
        }
    }
}