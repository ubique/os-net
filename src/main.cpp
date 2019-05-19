#include "sftp_server.h"
#include <iostream>

static const std::string USAGE =
    R"SEQ(
Simple synchronous SFTP server.
Usage: server [address [port]].
Default address is 127.0.0.1
Default port is 115. Note, that usage of this port may require superuser privileges.
All interaction is logged into stdout.
For convenience, some accounts are already created:
User: ubique, account: ubique, password: ubique
User: knisht, account: knisht1, password: knisht2
)SEQ";

int main(int argc, char *argv[])
{
    std::cout << USAGE << std::endl;
    try {
        std::string address = argc < 2 ? "127.0.0.1" : argv[1];
        std::string port = argc < 3 ? "115" : argv[2];
        sftp_server server(address, port);
        server.add_account("ubique", "ubique", "ubique");
        server.add_account("ubique", "thinks_that", "students_must_suffer");
        server.add_account("knisht", "knisht1", "knisht2");
        server.run();
    } catch (std::exception const &e) {
        std::cout << e.what() << std::endl;
    }
}
