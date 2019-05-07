//
// Created by max on 01.05.19.
//

#include "server.h"

int main(int argc, char **argv) {
    server server{(argc != 1 ? argv[1] : "/tmp/server.socket")};

    server.start();
}