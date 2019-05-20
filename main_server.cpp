#include <iostream>
#include "server.h"

int main() {
    in_port_t  port = htons(8888);

    try {
        server sr(port);
        sr.start();
    } catch (std::runtime_error& e) {
        perror(e.what());
        return 1;
    }
}