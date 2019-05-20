#include <iostream>
#include <arpa/inet.h>
#include "client.h"

int main(int argc, char const *argv[]) {
    uint32_t address = inet_addr("127.0.0.1");
    in_port_t port = htons(8888);
    std::string message;
    if (argc > 1) {
        message = argv[1];
    } else {
        message = "default";
    }

    try {
        client cl(address, port);
        cl.echo(message);
    } catch (std::runtime_error &e) {
        perror(e.what());
        return 1;
    }
}