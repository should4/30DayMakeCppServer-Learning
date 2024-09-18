#include "src/Socket.h"
#include "src/InetAddress.h"
#include "src/Buffer.h"
#include "src/util.h"

#include <string.h>
#include <unistd.h>
int main()
{
    const char *ip = "127.0.0.1";
    int port = 8888;

    Socket *client_sock = new Socket();
    InetAddress *client_addr = new InetAddress(ip, port);
    client_sock->connect(client_addr); // 连接服务器

    Buffer *readBuffer = new Buffer();  // 读缓冲区
    Buffer *writeBuffer = new Buffer(); // 写缓冲区

    while (true)
    {
        writeBuffer->getline(); // 从键盘输入待写字符串
        printf("client %d,send %s\n", client_sock->getFd(), writeBuffer->c_str());
        errif(write(client_sock->getFd(), writeBuffer->c_str(), writeBuffer->size()) == -1, "write error");

        // 开始读取服务器端发送的数据
        char buffer[1024];
        int already_read = 0; // 记录已经读取的数据字节数
        while (true)
        {
            bzero(buffer, sizeof(buffer));
            int readBytes = read(client_sock->getFd(), buffer, sizeof(buffer));
            if (readBytes > 0)
            {
                readBuffer->append(buffer, readBytes);
                already_read += readBytes;
                printf("client %d,read %s\n", client_sock->getFd(), buffer);
            }
            else if (readBytes == 0) // EOF
            {
                printf("client %d has disconnected\n", client_sock->getFd());
                exit(EXIT_SUCCESS);
            }

            if (already_read >= writeBuffer->size())
            {
                printf("client %d,already read %s\n", client_sock->getFd(), readBuffer->c_str());
                break;
            }
        }
    }
    delete client_sock;
    delete client_addr;

    return 0;
}