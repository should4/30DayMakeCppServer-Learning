#include "Socket.h"
#include "InetAddress.h"
#include "util.h"

#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>

Socket::Socket() : fd(-1)
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd == -1, "socket create error");
}

Socket::Socket(int fd) : fd(fd)
{
    errif(fd == -1, "socket create error");
}

Socket::~Socket()
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

void Socket::bind(InetAddress *addr)
{
    errif(::bind(fd, (struct sockaddr *)&addr->sock_addr, addr->sock_len) == -1, "bind error");
}

void Socket::listen()
{
    errif(::listen(fd, BACK_BLOG) == -1, "listen error");
}

int Socket::accept(InetAddress *addr)
{
    // printf("server fd %d ,addr_len %d,start to accept\n", fd, addr->sock_len);
    int client_fd = ::accept(fd, (sockaddr *)&addr->sock_addr, &addr->sock_len);
    // printf("finished accept\n");
    errif(client_fd == -1, "accept error");
    return client_fd;
}

void Socket::setnonblock()
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::getFd()
{
    return fd;
}