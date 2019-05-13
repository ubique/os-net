//
// Created by rsbat on 5/12/19.
//

#include "POP_server.h"

int main(int argc, char* argv[]) {
    POP_server server("127.0.0.1", "12345");

    server.run();
}