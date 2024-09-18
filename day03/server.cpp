#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include "util.h"
#include <string.h>
#include <sys/epoll.h>
#include <errno.h>

#define BACK_BLOG 5      // 定义服务器监听队列大小
#define MAX_EVENTS 10    // 定义 epoll 表监听文件描述符的大小
#define BUFFER_SIZE 1024 // 定义接受缓冲区大小

void setNoneBlock(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main(int argc, char *argv[])
{
    errif(argc <= 2, "usag: ip port");

    char *ip = argv[1];
    int port = atoi(argv[2]);

    int serv_fd = socket(PF_INET, SOCK_STREAM, 0);
    errif(serv_fd == -1, "server create error");

    struct sockaddr_in listen_addr;
    bzero(&listen_addr, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &listen_addr.sin_addr);

    errif(bind(serv_fd, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) == -1, "bind error");

    errif(listen(serv_fd, BACK_BLOG) == -1, "listen error");

    // 定义 epoll 表
    int epfd = epoll_create1(0);
    // 定义 event 结构体
    struct epoll_event events[MAX_EVENTS], ev;
    ev.events = EPOLLIN;
    ev.data.fd = serv_fd;

    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_fd, &ev); // 将 服务器监听描述符 添加到 epoll 表中
    while (true)
    {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1); // 使用阻塞的方式等待 epoll 表中监听的文件描述符的状态是否发生改变

        // 遍历所有表中的文件描述符
        for (int i = 0; i < nfds; ++i)
        {
            if (events[i].data.fd == serv_fd) // 如果当前服务器文件描述符上出现事件
            {
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                int client_fd = accept(serv_fd, (struct sockaddr *)&client_addr, &client_addr_len);
                errif(client_fd == -1, "accept client error");

                char src_ip[INET_ADDRSTRLEN];
                int src_port = ntohs(client_addr.sin_port);
                inet_ntop(AF_INET, &client_addr.sin_addr, src_ip, sizeof(src_ip));
                printf("new client fd %d,ip %s,port %d\n", client_fd, src_ip, src_port);

                // 将连接到的客户端文件描述符 放入 epoll 监听表中
                ev.events = EPOLLIN | EPOLLET; // 使用 ET 模式处理客户端连接
                ev.data.fd = client_fd;
                setNoneBlock(client_fd); // 将客户端 I/O 读取方式设置为非阻塞方式 以配合 ET 模式
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
            }
            else if (events[i].events & EPOLLIN) // 发生事件的是客户端，并且是可读事件（EPOLLIN）
            {
                char buffer[BUFFER_SIZE]; // 接受缓冲区
                int client_fd = events[i].data.fd;
                while (true)
                {
                    bzero(buffer, BUFFER_SIZE);
                    int read_bytes = read(client_fd, buffer, BUFFER_SIZE);
                    if (read_bytes > 0)
                    {
                        printf("client %d ,bytes %d,recv %s\n", client_fd, read_bytes, buffer);
                        write(client_fd, buffer, BUFFER_SIZE);
                    }
                    else if (read_bytes == -1 && errno == EINTR) // 客户端正常终端 继续读取
                    {
                        printf("continue reading");
                        continue;
                    }
                    else if (read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 非阻塞IO，这个条件表示数据全部读取完毕
                    {
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    }
                    else if (read_bytes == 0) // EOF事件，一般表示客户端断开连接
                    {
                        printf("EOF, client fd %d disconnected\n", client_fd);
                        close(client_fd); // 关闭socket会自动将文件描述符从epoll树上移除
                        break;
                    }
                    else
                    {
                        printf("something else happened\n");
                        break;
                    }
                }
            }
        }
    }

    close(serv_fd);
    return 0;
}