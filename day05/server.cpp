#include "Epoll.h"
#include "Channel.h"
#include "util.h"
#include "Socket.h"
#include "InetAddress.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void handleEvent(int fd);

int main(int argc, char *argv[])
{
    errif(argc <= 2, "usage: ip port");

    Socket *serv_sock = new Socket();
    InetAddress serv_addr(argv[1], atoi(argv[2]));

    serv_sock->bind(&serv_addr);
    serv_sock->listen();

    Epoll *ep = new Epoll();
    Channel *serv_channel = new Channel(ep, serv_sock->getFd());
    serv_channel->setenableReading(EPOLLIN); // 将服务器 channel 添加到 epoll 监听树中

    while (true)
    {
        std::vector<Channel *> active_channels = ep->poll();
        for (int i = 0; i < active_channels.size(); ++i)
        {
            auto channel = active_channels[i];
            if (channel->getFd() == serv_sock->getFd())
            {
                InetAddress client_addr;
                int client_fd = serv_sock->accept(&client_addr);
                Socket *client_sock = new Socket(client_fd);
                client_sock->setnonblock(); // 将客户端数据读取设置为非阻塞 I/O 模式
                Channel *temp = new Channel(ep, client_sock->getFd());
                temp->setenableReading();
                printf("new client %d,ip %s,port %d\n", client_fd, inet_ntoa(client_addr.sock_addr.sin_addr), ntohs(client_addr.sock_addr.sin_port));
            }
            else if (channel->getEvents() & EPOLLIN) // 可读事件
            {
                handleEvent(channel->getFd());
            }
            else
            {
                printf("其他事件\n");
            }
        }
    }

    delete serv_sock; // 关闭服务器 其析构函数自动将 fd 关闭，fd 关闭 epoll 树自动将其从树中剥离
    delete ep;
    return 0;
}

void handleEvent(int fd)
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
            printf("系统终端正常中断，继续\n");
            continue;
        }
        else if (readbytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            printf("一次性读取完成\n");
            break;
        }
        else if (readbytes == 0)
        {
            printf("客户端 %d 断开\n", fd);
            break;
        }
        else
        {
            printf("一些未知的错误\n");
            break;
        }
    }
}