#include "../utils/util.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BACKLOG_SIZE 5
#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    errif(argc <= 2, "usage : ip port");

    char *ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &serv_addr.sin_addr);

    int serv_fd = socket(PF_INET, SOCK_STREAM, 0);
    errif(serv_fd == -1, "socket create error");

    errif(bind(serv_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1, "bind error");

    errif(listen(serv_fd, BACKLOG_SIZE) == -1, "listen error");

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    bzero(&client_addr, client_addr_len);
    int client_fd = accept(serv_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    errif(client_fd == -1, "accept error");

    printf("new client fd %d, IP: %s, Port: %d \n", client_fd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    while (true)
    {
        char buf[BUFFER_SIZE];
        ssize_t read_bytes = read(client_fd, buf, sizeof(buf));

        if (read_bytes > 0)
        {
            printf("message from client fd %d: %s\n", client_fd, buf);
            write(client_fd, buf, sizeof(buf));
        }
        else if (read_bytes == 0)
        {
            printf("client fd %d finished read,disconnected\n", client_fd);
            close(client_fd);
            break;
        }
        else if (read_bytes == -1)
        {
            close(client_fd);
            errif(true, "socket read error");
        }
        close(serv_fd);
        return 0;
    }
}