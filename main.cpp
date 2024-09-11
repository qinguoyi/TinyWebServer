#include "config.h"
#include "webserver.h"

int main(int argc, char *argv[])
{
    WebServer server;
    server.init(argc,argv);
    server.run();

    return 0;
}