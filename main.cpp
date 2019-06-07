#include "server.h"

#include <iostream>
#include <limits>
#include <cstring>
#include <signal.h>

constexpr static char const* manpage = "Usage: net [port]";

static volatile sig_atomic_t termination_marker = 0;

void sigint_handler(int)
{
    termination_marker = 1;
}

int main(int argc, char** argv)
{
    uint16_t port = 1080;

    if (argc == 2)
    {
        auto conv = strtol(argv[1], nullptr, 10);

        if (errno != 0)
        {
            printf("Unrecognized port: %s\n%s\n", strerror(errno), manpage);
            return -1;
        }

        if (conv < 0 || conv > std::numeric_limits<uint16_t>::max())
        {
            printf("Invalid port, not in a range [0, 65536]");
            return -1;
        }

        port = static_cast<uint16_t>(conv);
    }

    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = sigint_handler;
    sigaction(SIGINT, &action, nullptr);

    socks5_server server(port, termination_marker);

    server.run();
    return 0;
}
