#include "InetAddress.h"
#include <string.h>

InetAddress::InetAddress() {}
InetAddress::InetAddress(char *ip, int port)
{
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &sock_addr.sin_addr);
    sock_len = sizeof(sock_addr);
}