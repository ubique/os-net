#include "tftp_peer.h"

#include "tftp_ec.h"
#include "tftp_packet.h"
#include "utils.h"

#include <errno.h>   // errno
#include <netdb.h>   // getaddrinfo, getnameinfo
#include <stdbool.h> // bool
#include <stdio.h>   // fdopen, FILE
#include <unistd.h>  // write

#define DEFAULT_NODE "127.0.0.1"
#define RECV_TIMEOUT_SECS 5
#define MAX_RECV_ATTEMPTS 5
#define MAX_SEND_ATTEMPTS 5

struct tftp_peer {
    int sfd;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;

    char packet_buf[PACKET_SIZE];
    size_t packet_buf_len;
};

struct tftp_peer *
tftp_peer_create(char const *node, char const *port, tftp_ec *ec) {
    if (node == NULL) {
        node = DEFAULT_NODE;
    }
    struct addrinfo hints = {.ai_family = AF_UNSPEC,
                             .ai_socktype = SOCK_DGRAM,
                             .ai_protocol = IPPROTO_UDP,
                             .ai_flags = AI_NUMERICSERV};
    struct addrinfo *result;
    int gai_ec = getaddrinfo(node, port, &hints, &result);
    if (gai_ec != 0) {
        err_log("getaddrinfo failed: %s", gai_strerror(gai_ec));
        ec_set(ec, tftp_EGAI);
        return NULL;
    }

    struct tftp_peer *peer = calloc(1, sizeof(struct tftp_peer));
    if (peer == NULL) {
        err_log("calloc failed: %s", strerror(errno));
        ec_set(ec, tftp_ENOMEM);
        free(peer);
        return NULL;
    }
    bool socket_ok = false;
    for (struct addrinfo *rp = result; rp != NULL; rp = rp->ai_next) {
        peer->sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (peer->sfd != -1) {
            socket_ok = true;
            memcpy(&peer->peer_addr, rp->ai_addr, rp->ai_addrlen);
            peer->peer_addr_len = rp->ai_addrlen;
            break;
        }
    }
    freeaddrinfo(result);
    if (!socket_ok) {
        err_log("could not create socket");
        ec_set(ec, tftp_ESOCKET);
        free(peer);
        return NULL;
    }

    // set receive timeout
    struct timeval tv = {RECV_TIMEOUT_SECS, 0};
    socklen_t optlen = sizeof tv;
    if (setsockopt(peer->sfd, SOL_SOCKET, SO_RCVTIMEO, &tv, optlen) == -1) {
        err_log("setsockopt failed: %s", strerror(errno));
        ec_set(ec, tftp_ESETSOCKOPT);
        close(peer->sfd);
        free(peer);
        return NULL;
    }

    return peer;
}

void
tftp_peer_destroy(struct tftp_peer *peer) {
    if (close(peer->sfd) == -1) {
        err_log("close failed: %s", strerror(errno));
    }
    free(peer);
}

int
resend_from_buf(struct tftp_peer const *client, tftp_ec *ec) {
    int rc = sendto(client->sfd, client->packet_buf, client->packet_buf_len, 0,
                    (struct sockaddr const *)&client->peer_addr,
                    client->peer_addr_len);
    if (rc == -1) {
        err_log("sendto failed: %s", strerror(errno));
        ec_set(ec, tftp_ESEND);
        return -1;
    }
    return 0;
}

int
send_packet(struct tftp_peer *client, struct tftp_packet const *packet,
            tftp_ec *ec) {
    ssize_t pack_size = packet_to_bytes(packet, client->packet_buf, ec);
    if (pack_size == -1) {
        return -1;
    }
    client->packet_buf_len = pack_size;
    return resend_from_buf(client, ec);
}

int
send_rrq(struct tftp_peer *client, char const *filename, tftp_ec *ec) {
    struct tftp_packet packet;
    if (make_rrq_or_wrq(tftp_RRQ, filename, &packet, ec) == -1) {
        return -1;
    }
    return send_packet(client, &packet, ec);
}

int
send_wrq(struct tftp_peer *client, char const *filename, tftp_ec *ec) {
    struct tftp_packet packet;
    if (make_rrq_or_wrq(tftp_WRQ, filename, &packet, ec) == -1) {
        return -1;
    }
    return send_packet(client, &packet, ec);
}

int
send_data(struct tftp_peer *peer, unsigned block_no, char buf[static DATA_SIZE],
          size_t buf_size, tftp_ec *ec) {
    struct tftp_packet packet;
    make_data(block_no, buf, buf_size, &packet);
    return send_packet(peer, &packet, ec);
}

int
send_ack(struct tftp_peer *peer, unsigned block_no, tftp_ec *ec) {
    struct tftp_packet packet;
    make_ack(block_no, &packet);
    return send_packet(peer, &packet, ec);
}

int
send_error(struct tftp_peer *peer, tftp_packet_error error,
           char const *error_msg, tftp_ec *ec) {
    struct tftp_packet packet;
    make_error(error, error_msg, ec, &packet);
    return send_packet(peer, &packet, ec);
}

int
receive_bytes(struct tftp_peer *peer, char buf[static PACKET_SIZE],
              ssize_t *nread, tftp_ec *ec) {
    *nread =
        recvfrom(peer->sfd, buf, PACKET_SIZE, 0,
                 (struct sockaddr *)&peer->peer_addr, &peer->peer_addr_len);
    if (*nread != -1) {
        return 0;
    }
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        ec_set(ec, tftp_EAGAIN);
    } else {
        err_log("recvfrom failed: %s", strerror(errno));
        ec_set(ec, tftp_ERECV);
    }
    return -1;
}

int
receive_packet_with_retry(struct tftp_peer *peer, struct tftp_packet *packet,
                          tftp_ec *ec) {
    char buf[PACKET_SIZE];
    ssize_t nread = -1;
    for (size_t attempt_no = 0; attempt_no < MAX_RECV_ATTEMPTS; ++attempt_no) {
        if (receive_bytes(peer, buf, &nread, ec) == 0) {
            break;
        }
        if (*ec == tftp_EAGAIN) {
            resend_from_buf(peer, ec);
        } else {
            return -1;
        }
    }
    if (nread == -1) {
        err_log("timeout expired, no reply");
        ec_set(ec, tftp_ENO_REPLY);
        return -1;
    }
    return bytes_to_packet(buf, nread, packet, ec);
}

enum expect_type { EXPECT_DATA, EXPECT_ACK };

int
expect_packet(struct tftp_peer *peer, unsigned block_no,
              enum expect_type expect, struct tftp_packet *packet,
              tftp_ec *ec) {
    for (size_t attempt_no = 0; attempt_no < MAX_SEND_ATTEMPTS; ++attempt_no) {
        if (receive_packet_with_retry(peer, packet, ec) == -1) {
            err_log("receive_packet_with_retry failed");
            return -1;
        }
        if (packet->opcode == tftp_ERROR) {
            err_log("received error package: %s: %s",
                    tftp_packet_error_to_string[packet->error_code],
                    packet->error_msg);
            ec_set(ec, tftp_EERROR_PACKET_RECEIVED);
            return -1;
        }
        unsigned packet_block_no;
        switch (expect) {
        case EXPECT_ACK:
            if (packet->opcode != tftp_ACK) {
                err_log("expected ack opcode, but received %d", packet->opcode);
                ec_set(ec, tftp_EINVALID_PACKET);
                return -1;
            }
            packet_block_no = packet->ack_block_no;
            break;
        case EXPECT_DATA:
            if (packet->opcode != tftp_DATA) {
                err_log("expected data opcode, but received %d",
                        packet->opcode);
                ec_set(ec, tftp_EINVALID_PACKET);
                return -1;
            }
            packet_block_no = packet->data_block_no;
            break;
        default:
            unreachable("unexpected enum expect_type value");
        }

        if (packet_block_no == block_no) {
            return 0; // success
        }
        if (packet_block_no == block_no - 1) {
            // might overflow, then equality is false
            resend_from_buf(peer, ec);
        } else {
            err_log("unexpected block #, expected %u or %u - 1, but got %u",
                    block_no, block_no, packet_block_no);
            ec_set(ec, tftp_EINVALID_PACKET);
            return -1;
        }
    }

    err_log("last packet keeps getting lost");
    ec_set(ec, tftp_EPACKET_LOST);
    return -1;
}

int
expect_ack(struct tftp_peer *peer, unsigned block_no, tftp_ec *ec) {
    struct tftp_packet packet;
    return expect_packet(peer, block_no, EXPECT_ACK, &packet, ec);
}

int
expect_data(struct tftp_peer *peer, unsigned block_no,
            char buf[static DATA_SIZE], size_t *buf_size, tftp_ec *ec) {
    struct tftp_packet packet;
    if (expect_packet(peer, block_no, EXPECT_DATA, &packet, ec) == -1) {
        err_log("expect_packet failed");
        return -1;
    }
    assert(packet.opcode == tftp_DATA);
    assert(packet.data_size <= DATA_SIZE);
    *buf_size = packet.data_size;
    memcpy(buf, packet.data, *buf_size);
    return 0;
}

int
peer_send_file(struct tftp_peer *peer, FILE *file, tftp_ec *ec) {
    unsigned block_no = 1;
    bool last_chunk = false;
    while (!last_chunk) {
        char data_buf[DATA_SIZE];
        size_t nread = fread(data_buf, sizeof(char), DATA_SIZE, file);
        int errnum = ferror(file);
        if (errnum != 0) {
            err_log("fread failed: %s", strerror(errnum));
            ec_set(ec, tftp_EIO_PROBLEM);
            return -1;
        }
        if (feof(file)) {
            last_chunk = true;
        }
        if (send_data(peer, block_no, data_buf, nread, ec) == -1) {
            info_log("send_data failed at block_no # %u", block_no);
        }
        if (expect_ack(peer, block_no++, ec) == -1) {
            err_log("expect_ack failed");
            return -1;
        }
    }
    return 0;
}

int
tftp_client_send_file(struct tftp_peer *client, FILE *file,
                      char const *remote_filename, tftp_ec *ec) {
    if (send_wrq(client, remote_filename, ec) == -1) {
        err_log("send_wrq failed");
        return -1;
    }
    if (expect_ack(client, 0, ec) == -1) {
        err_log("expect_ack failed");
        return -1;
    }
    int rc = connect(client->sfd, (struct sockaddr const *)&client->peer_addr,
                     client->peer_addr_len);
    if (rc == -1) {
        err_log("connect failed: %s", strerror(errno));
        ec_set(ec, tftp_ECONNECT);
        return -1;
    }
    return peer_send_file(client, file, ec);
}

int
server_send_file(struct tftp_peer *server, FILE *file, enum tftp_mode mode,
                 tftp_ec *ec) {
    if (mode != tftp_OCTET) {
        err_log("only octet mode is supported");
        ec_set(ec, tftp_EMODE_NOT_SUPPORTED);
        return -1;
    }
    int rc = connect(server->sfd, (struct sockaddr const *)&server->peer_addr,
                     server->peer_addr_len);
    if (rc == -1) {
        err_log("connect failed: %s", strerror(errno));
        ec_set(ec, tftp_ECONNECT);
        return -1;
    }
    return peer_send_file(server, file, ec);
}

int
peer_recv_file(struct tftp_peer *peer, FILE *file, tftp_ec *ec) {
    unsigned block_no = 1;
    bool last_chunk = false;
    char data_buf[DATA_SIZE];
    size_t nread;
    while (!last_chunk) {
        if (expect_data(peer, block_no, data_buf, &nread, ec) == -1) {
            err_log("expect_data failed");
            return -1;
        }
        fwrite(data_buf, sizeof(char), nread, file);
        int errnum = ferror(file);
        if (errnum != 0) {
            err_log("fwrite failed: %s", strerror(errnum));
            tftp_packet_error error;
            if (errnum == EFBIG || errnum == ENOSPC) {
                error = tftp_packet_EDISK_FULL;
            } else {
                error = tftp_packet_ENOT_DEFINED;
            }
            if (send_error(peer, error, strerror(errnum), ec) == -1) {
                err_log("send_error failed");
            }
            return -1;
        }
        if (nread < DATA_SIZE) {
            last_chunk = true;
        }
        if (send_ack(peer, block_no++, ec) == -1) {
            err_log("send_ack failed at block #%u", block_no - 1);
        }
    }
    return 0;
}

int
tftp_client_recv_file(struct tftp_peer *client, FILE *file,
                      char const *remote_filename, tftp_ec *ec) {
    if (send_rrq(client, remote_filename, ec) == -1) {
        err_log("send_rrq failed");
        return -1;
    }
    return peer_recv_file(client, file, ec);
}

int
server_recv_file(struct tftp_peer *server, FILE *file, enum tftp_mode mode,
                 tftp_ec *ec) {
    if (mode != tftp_OCTET) {
        err_log("only octet mode is supported");
        ec_set(ec, tftp_EMODE_NOT_SUPPORTED);
        return -1;
    }
    int rc = connect(server->sfd, (struct sockaddr const *)&server->peer_addr,
                     server->peer_addr_len);
    if (rc == -1) {
        err_log("connect failed: %s", strerror(errno));
        ec_set(ec, tftp_ECONNECT);
        return -1;
    }
    if (send_ack(server, 0, ec) == -1) {
        err_log("send_ack failed");
        return -1;
    }
    return peer_recv_file(server, file, ec);
}

int
add_session(struct tftp_packet *packet, const char *server_node,
            struct sockaddr_storage const peer_addr, socklen_t peer_addr_len) {
    int pid = fork();
    if (pid == -1) {
        err_log("couldn't fork to serve request: %s", strerror(errno));
        return -1;
    } else if (pid == 0) {
        bool is_wrq = packet->opcode == tftp_WRQ;
        info_log("%s '%s' request received", is_wrq ? "wrq" : "rrq",
                 packet->filename);
        tftp_ec ec;
        struct tftp_peer *SCOPED_PEER new_server =
            tftp_peer_create(server_node, NULL, &ec);
        new_server->peer_addr = peer_addr;
        new_server->peer_addr_len = peer_addr_len;
        FILE *file SCOPED_FILE = fopen(packet->filename, is_wrq ? "w" : "r");
        if (file == NULL) {
            send_error(new_server, tftp_packet_ENOT_DEFINED, strerror(errno),
                       &ec); // TODO: send correct error
            err_log("fopen failed: %s", strerror(errno));
            ec = tftp_EFOPEN;
            return -1;
        }
        if (is_wrq) {
            server_recv_file(new_server, file, packet->mode, &ec);
        } else {
            assert(packet->opcode == tftp_RRQ);
            server_send_file(new_server, file, packet->mode, &ec);
        }
        info_log("%s '%s' request served", is_wrq ? "wrq" : "rrq",
                 packet->filename);
        exit(0);
    }
    return 0;
}

int
tftp_server_listen(struct tftp_peer *server, tftp_ec *ec) {
    int rc = bind(server->sfd, (struct sockaddr const *)&server->peer_addr,
                  server->peer_addr_len);
    if (rc == -1) {
        err_log("bind failed: %s", strerror(errno));
        ec_set(ec, tftp_EBIND);
        return -1;
    }

    char host_buf[NI_MAXHOST];
    rc = getnameinfo((const struct sockaddr *)&server->peer_addr,
                     server->peer_addr_len, host_buf, sizeof(host_buf), NULL, 0,
                     NI_NAMEREQD | NI_DGRAM);
    if (rc != 0) {
        err_log("getnameinfo failed: %s", gai_strerror(rc));
        ec_set(ec, tftp_ECANT_RESOLVE_HOST);
        return -1;
    }

    char buf[PACKET_SIZE];
    ssize_t nread;
    while (true) {
        if (receive_bytes(server, buf, &nread, ec) == 0) {
            struct tftp_packet packet;
            if (bytes_to_packet(buf, nread, &packet, ec) == -1) {
                return -1;
            }
            if (packet.opcode != tftp_WRQ && packet.opcode != tftp_RRQ) {
                err_log("incorrect opcode: %d, expected rrq or wrq; dropping "
                        "the packet",
                        packet.opcode);
                continue;
            }
            if (add_session(&packet, host_buf, server->peer_addr,
                            server->peer_addr_len) == -1) {
                err_log("failed to add session");
            }
        }
    }
}
