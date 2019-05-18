#include <stdio.h> // fprintf, stderr

#include "tftp.h"

#define PRINT_USAGE() fprintf(stderr, "USAGE: %s [address [port]]", argv[0]);

char const *
arg_or_default(unsigned argc, char *argv[], unsigned arg,
               char const *default_value) {
    return arg < argc ? argv[arg] : default_value;
}

int
main(int argc, char *argv[]) {
    if (argc > 3) {
        PRINT_USAGE();
    }
    unsigned arg = 0;
    char const *server_node = arg_or_default(argc, argv, ++arg, NULL);
    char const *port = arg_or_default(argc, argv, ++arg, TFTP_DEFAULT_PORT);
    tftp_ec ec;
    if (tftp_listen(server_node, port, &ec) == -1) {
        fprintf(stderr, "tftp_listen failed: %s\n", tftp_strerror(ec));
    }
}
