#include "tftp.h"

#include "tftp_peer.h"
#include "utils.h"

int
tftp_listen(char const *server_node, char const *port, tftp_ec *ec) {
    struct tftp_peer *SCOPED_PEER server =
        tftp_peer_create(server_node, port, ec);
    if (server == NULL) {
        err_log("tftp_peer_create failed");
        return -1;
    }
    if (tftp_server_listen(server, ec) == -1) {
        err_log("tftp_server_listen failed");
        return -1;
    }
    return 0;
}

int
tftp_write(char const *server_node, char const *port, FILE *file,
           char const *remote_filename, tftp_ec *ec) {
    struct tftp_peer *SCOPED_PEER client =
        tftp_peer_create(server_node, port, ec);
    if (client == NULL) {
        err_log("tftp_peer_create failed");
        return -1;
    }
    if (tftp_client_send_file(client, file, remote_filename, ec) == -1) {
        err_log("tftp_client_send_file failed");
        return -1;
    }
    return 0;
}

int
tftp_read(char const *server_node, char const *port, FILE *file,
          char const *remote_filename, tftp_ec *ec) {
    struct tftp_peer *SCOPED_PEER client =
        tftp_peer_create(server_node, port, ec);
    if (client == NULL) {
        err_log("tftp_peer_create failed");
        return -1;
    }
    if (tftp_client_recv_file(client, file, remote_filename, ec) == -1) {
        err_log("tftp_client_recv_file failed");
        return -1;
    }
    return 0;
}
