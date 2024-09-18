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
    int accept(InetAddress *addr);

    // 客户端
    void connect(InetAddress *addr);

    // 设置非阻塞
    void setnonblock();

    int getFd();
};