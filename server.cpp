#include "Server.h"

int main(int argc, char* argv[]) {
    Server server(8888);
    server.run();
}