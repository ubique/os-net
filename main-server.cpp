#include <memory>

//
// Created by max on 01.05.19.
//

#include <csignal>
#include "server.h"

std::unique_ptr<server> m_server;

void f_signal(UNUSED int a) {
    std::cout << "Exiting..." << std::endl;
    m_server->interrupt();
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    signal(SIGINT, f_signal);
    m_server = std::make_unique<server>((argc != 1 ? argv[1] : "/tmp/server.socket"));

    m_server->start();
}