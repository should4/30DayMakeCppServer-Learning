#include "InetAddress.h"
#include <string.h>

// BUG : here 没有初始化（addrlen= sizeof（addr））
// InetAddress::InetAddress() {}

// after solved
InetAddress::InetAddress() : sock_len(sizeof(sock_addr))
{
    bzero(&sock_addr, sock_len);
}

InetAddress::InetAddress(const char *ip, int port)
{
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &sock_addr.sin_addr);
    sock_len = sizeof(sock_addr);
}

InetAddress::~InetAddress()
{

}