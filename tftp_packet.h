#ifndef TFTP_TFTP_PACKET_H
#define TFTP_TFTP_PACKET_H

#include "tftp_ec.h"

#include <inttypes.h>  // uint16_t
#include <stddef.h>    // size_t
#include <sys/types.h> // ssize_t

#define DATA_SIZE 512
#define PACKET_SIZE (DATA_SIZE + 4)
// max payload: 2B opcode + 2B block # + 512B data

typedef enum {
    tftp_RRQ = 1,
    tftp_WRQ = 2,
    tftp_DATA = 3,
    tftp_ACK = 4,
    tftp_ERROR = 5
} tftp_opcode;

enum tftp_mode { tftp_NETASCII, tftp_OCTET, tftp_MAIL };
static char const *const tftp_mode_to_string[] = {"netascii", "octet", "mail"};

typedef enum {
    tftp_packet_ENOT_DEFINED = 0,
    tftp_packet_EFILE_NOT_FOUND = 1,
    tftp_packet_EACCESS_VIOLATION = 2,
    tftp_packet_EDISK_FULL = 3,
    tftp_packet_EILLEGAL_OP = 4,
    tftp_packet_EUNKNOWN_TID = 5,
    tftp_packet_EFILE_EXISTS = 6,
    tftp_packet_ENO_SUCH_USER = 7
} tftp_packet_error;

static char const *const tftp_packet_error_to_string[] = {
    "error not defined",
    "file not found",
    "access violation",
    "disk full or allocation exceeded",
    "illegal TFTP operation",
    "file already exists",
    "no such user"};

struct tftp_packet {
    tftp_opcode opcode;
    union {
        // rrq/wrq
        struct {
            // has to be zero terminated.
            // size might be slightly more then fits,
            // so checking strlen is necessary
            char filename[PACKET_SIZE];
            enum tftp_mode mode;
        };
        // data
        struct {
            uint16_t data_block_no;
            size_t data_size;
            char data[DATA_SIZE];
        };
        // ack
        uint16_t ack_block_no;
        // error
        struct {
            tftp_packet_error error_code;
            // has to be zero terminated.
            // size might be slightly more then fits,
            // so checking strlen is necessary
            char error_msg[PACKET_SIZE];
        };
    };
};

ssize_t
packet_to_bytes(struct tftp_packet const *packet, char buf[static PACKET_SIZE],
                tftp_ec *ec);

int
bytes_to_packet(char const buf[static PACKET_SIZE], size_t buf_len,
                struct tftp_packet *packet, tftp_ec *ec);

int
make_rrq_or_wrq(tftp_opcode rrq_or_wrq, char const *filename,
                struct tftp_packet *packet, tftp_ec *ec);

void
make_data(unsigned block_no, char buf[512], size_t buf_size,
          struct tftp_packet *packet);

void
make_ack(unsigned block_no, struct tftp_packet *packet);

int
make_error(tftp_packet_error error_code, char const *error_msg, tftp_ec *ec,
           struct tftp_packet *packet);

#endif // TFTP_TFTP_PACKET_H
