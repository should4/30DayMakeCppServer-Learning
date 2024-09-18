#pragma once
#include <arpa/inet.h>

class InetAddress
{
public:
    struct sockaddr_in sock_addr;
    socklen_t sock_len;
    InetAddress();
    InetAddress(char *ip, int port);
};