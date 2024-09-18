#include "src/EventLoop.h"
#include "src/Server.h"

int main()
{
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);
    loop->loop(); // 运行 mainReactor
    return 0;
}