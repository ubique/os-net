#ifndef TFTP_TFTP_H
#define TFTP_TFTP_H

#include <stdio.h> // FILE

#include "tftp_ec.h"

#define TFTP_DEFAULT_PORT "25000"

int
tftp_listen(char const *server_node, char const *port, tftp_ec *ec);

int
tftp_write(char const *server_node, char const *port, FILE *file,
           char const *remote_filename, tftp_ec *ec);

int
tftp_read(char const *server_node, char const *port, FILE *file,
          char const *remote_filename, tftp_ec *ec);

#endif // TFTP_TFTP_H
