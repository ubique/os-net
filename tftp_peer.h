#ifndef TFTP_TFTP_PEER_H
#define TFTP_TFTP_PEER_H

#include "tftp_ec.h"
#include "tftp_packet.h"

#include <stddef.h>     // size_t
#include <stdio.h>      // FILE
#include <sys/socket.h> // socklen_t

struct tftp_peer;

struct tftp_peer *
tftp_peer_create(char const *node, char const *port, tftp_ec *ec);

void
tftp_peer_destroy(struct tftp_peer *peer);

int
tftp_client_send_file(struct tftp_peer *client, FILE *file,
                      char const *remote_filename, tftp_ec *ec);

int
tftp_client_recv_file(struct tftp_peer *client, FILE *file,
                      char const *remote_filename, tftp_ec *ec);

int
tftp_server_listen(struct tftp_peer *server, tftp_ec *ec);

#endif // TFTP_TFTP_PEER_H
