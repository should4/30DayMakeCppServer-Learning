#include "./src/EventLoop.h"
#include "./src/Server.h"
#include "./src/util.h"

int main(int argc, char *argv[])
{
    errif(argc != 3 && argc != 1, "usage: ip port");

    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);
    loop->loop();

    return 0;
}