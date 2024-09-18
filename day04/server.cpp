#include "InetAddress.h"
#include "Epoll.h"
#include "Socket.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 256

void handleReadEvent(int fd);

int main(int argc, char *argv[])
{
    errif(argc <= 2, "usage : ip port");

    Socket *server_sock = new Socket();
    InetAddress sock_addr(argv[1], atoi(argv[2]));
    server_sock->bind(&sock_addr);
    server_sock->listen();

    Epoll *epoll = new Epoll();
    epoll->addFd(server_sock->getFd(), EPOLLIN | EPOLLET); // 采用 ET 模式 将服务器 fd 放入 epoll 表中
    // printf("server_sock = %d\n", server_sock->getFd());

    while (true)
    {
        std::vector<epoll_event> active_events = epoll->poll(); // 默认阻塞的方式获取 epoll 表中可用 fd
        for (auto itr = active_events.begin(); itr != active_events.end(); ++itr)
        {
            // printf("sock = %d\n", itr->data.fd);
            if (itr->data.fd == server_sock->getFd()) // 服务器端监听到新的客户端连接
            {
                // printf("here\n");
                InetAddress client_addr;
                Socket *client_sock = new Socket(server_sock->accept(&client_addr)); // 会发生内存泄露！没有delete
                printf("new client %d,ip %s,port %d\n", client_sock->getFd(), inet_ntoa(client_addr.sock_addr.sin_addr), ntohs(client_addr.sock_addr.sin_port));
                client_sock->setnonblock();                            // 设置为非阻塞 I/O
                epoll->addFd(client_sock->getFd(), EPOLLIN | EPOLLET); // 客户端数据接收 I/O 采用 ET 模式
            }
            else if (itr->events & EPOLLIN) // 可读事件
            {
                handleReadEvent(itr->data.fd);
            }
            else
            {
                printf("something else happened!\n");
            }
        }
    }
    delete server_sock;
    return 0;
}

// 读事件处理函数
void handleReadEvent(int fd)
{
    char buffer[BUFFER_SIZE];
    while (true)
    {
        bzero(buffer, BUFFER_SIZE);
        int readbytes = read(fd, buffer, sizeof(buffer));
        if (readbytes > 0)
        {
            printf("client %d,recv %s\n", fd, buffer);
            write(fd, buffer, sizeof(buffer));
        }
        else if (readbytes == -1 && errno == EINTR)
        {
            printf("客户端正常中断,继续读取\n");
            continue;
        }
        else if (readbytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            printf("一次性读取结束\n");
            break;
        }
        else if (readbytes == 0)
        {
            printf("客户端断开\n");
            close(fd); // 关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
        else
        {
            printf("其他读取错误\n");
            break;
        }
    }
}