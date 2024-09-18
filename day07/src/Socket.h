#pragma once

#define BACK_BLOG 5

class InetAddress;

class Socket
{
private:
    int fd;

public:
    Socket();
    Socket(int fd);
    ~Socket();

    // 服务器端
    void bind(InetAddress *addr);
    void listen();
    void setnonblock();

    int accept(InetAddress *addr);

    int getFd();
};