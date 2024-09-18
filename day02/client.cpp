#include "../utils/util.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    errif(argc <= 2, "usage : ip port");

    char *ip = argv[1];
    int port = atoi(argv[2]);

    int client_fd = socket(PF_INET, SOCK_STREAM, 0);
    errif(client_fd == -1, "socket create error");

    struct sockaddr_in dst_addr;
    bzero(&dst_addr, sizeof(dst_addr));
    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &dst_addr.sin_addr);

    errif(connect(client_fd, (struct sockaddr *)&dst_addr, sizeof(dst_addr)) == -1, "connect error");

    while (true)
    {
        char buf[BUFFER_SIZE];
        bzero(buf, sizeof(buf)); // 清空缓冲区
        printf("input the message sent to server:");
        scanf("%s", buf);

        sleep(1);

        ssize_t write_bytes = write(client_fd, buf, sizeof(buf));
        if (write_bytes == -1)
        {
            printf("socket already disconnected,cannot wraite any more!\n");
            break;
        }

        bzero(buf, sizeof(buf));
        ssize_t read_bytes = read(client_fd, buf, sizeof(buf));
        if (read_bytes > 0)
        {
            printf("read from server : %s\n", buf);
        }
        else if (read_bytes == 0)
        {
            printf("server socket disconnected!\n");
            break;
        }
        else if (read_bytes == -1)
        {
            close(client_fd);
            errif(true, "socket read error");
        }
    }
    close(client_fd);
    return 0;
}