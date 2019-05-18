#include <assert.h>  // assert
#include <fcntl.h>   // open
#include <stdbool.h> // bool
#include <stdio.h>   // fprintf, stderr, fileno
#include <stdlib.h>  // exit
#include <string.h>  // strcmp

#include "tftp.h"
#include "tftp_peer.h"
#include "utils.h"

#define PRINT_USAGE()                                                          \
    fprintf(stderr,                                                            \
            "USAGE: %s -w/-r local-file remote-file server-address [port]",    \
            argv[0]);

char const *
arg_or_default(unsigned argc, char *argv[], unsigned arg,
               char const *default_value) {
    return arg < argc ? argv[arg] : default_value;
}

int
main(int argc, char *argv[]) {
    if (argc != 5 && argc != 6) {
        PRINT_USAGE();
        exit(EXIT_FAILURE);
    }
    unsigned arg = 0;
    char const *mode_s = argv[++arg];
    bool is_write;
    if (strcmp(mode_s, "-w") == 0) {
        is_write = true;
    } else if (strcmp(mode_s, "-r") == 0) {
        is_write = false;
    } else {
        PRINT_USAGE();
        exit(EXIT_FAILURE);
    }

    char const *local_filename = argv[++arg];
    char const *remote_filename = argv[++arg];
    char const *server_node = argv[++arg];
    char const *port = arg_or_default(argc, argv, ++arg, TFTP_DEFAULT_PORT);

    tftp_ec ec;
    int rc;
    if (is_write) {
        FILE *SCOPED_FILE file = fopen(local_filename, "r");
        rc = tftp_write(server_node, port, file, remote_filename, &ec);
    } else {
        FILE *SCOPED_FILE file = fopen(local_filename, "w");
        rc = tftp_read(server_node, port, file, remote_filename, &ec);
    }
    if (rc == -1) {
        fprintf(stderr, "operation failed: %s\n", tftp_strerror(ec));
    }
}
