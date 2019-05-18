#include "tftp_packet.h"

#include "utils.h"

#include <arpa/inet.h> // htons, ntohs
#include <assert.h>    // assert
#include <string.h>    // memcpy, strlen

int
put_string(char buf[], size_t *pos, char const *string, tftp_ec *ec) {
    size_t string_size = strlen(string) + 1;
    if (string_size + *pos > PACKET_SIZE) {
        err_log("put_string: string '%s' too long to send", string);
        ec_set(ec, tftp_EPACKET_TOO_LONG);
        return -1;
    }
    memcpy(buf + *pos, string, string_size);
    *pos += string_size;
    return 0;
}

void
put_number(char buf[], size_t *pos, uint16_t number) {
    number = htons(number);
    memcpy(buf + *pos, &number, 2);
    *pos += 2;
}

void
extract_string(char const buf[], size_t *pos, char *str_buf) {
    while (buf[*pos] != '\0') {
        *str_buf++ = buf[(*pos)++];
    }
    *str_buf = buf[(*pos)++]; // add '\0'
}

uint16_t
extract_number(char const buf[], size_t *pos) {
    uint16_t number;
    memcpy(&number, buf + *pos, 2);
    *pos += 2;
    return ntohs(number);
}

ssize_t
packet_to_bytes(struct tftp_packet const *packet, char buf[static PACKET_SIZE],
                tftp_ec *ec) {
    size_t pos = 0;
    put_number(buf, &pos, packet->opcode);
    switch (packet->opcode) {
    case tftp_RRQ:
    case tftp_WRQ:
        if (put_string(buf, &pos, packet->filename, ec) == -1) {
            return -1;
        }
        if (put_string(buf, &pos, tftp_mode_to_string[packet->mode], ec) ==
            -1) {
            return -1;
        }
        break;
    case tftp_DATA:
        put_number(buf, &pos, packet->data_block_no);
        memcpy(buf + pos, &packet->data, packet->data_size);
        pos += packet->data_size;
        break;
    case tftp_ACK:
        put_number(buf, &pos, packet->ack_block_no);
        break;
    case tftp_ERROR:
        put_number(buf, &pos, packet->error_code);
        if (put_string(buf, &pos, packet->error_msg, ec) == -1) {
            return -1;
        }
        break;
    }
    return pos;
}

int
bytes_to_packet(char const buf[static PACKET_SIZE], size_t buf_len,
                struct tftp_packet *packet, tftp_ec *ec) {
    if (buf_len < 2) {
        err_log("invalid packet, too short: %zd", buf_len);
        ec_set(ec, tftp_EINVALID_PACKET);
        return -1;
    }

    size_t pos = 0;
    packet->opcode = extract_number(buf, &pos);
    switch (packet->opcode) {
    case tftp_RRQ:
    case tftp_WRQ:
        extract_string(buf, &pos, packet->filename);
        char mode[PACKET_SIZE];
        extract_string(buf, &pos, mode);
        if (strcmp(mode, "netascii") == 0) {
            packet->mode = tftp_NETASCII;
        } else if (strcmp(mode, "octet") == 0) {
            packet->mode = tftp_OCTET;
        } else if (strcmp(mode, "mail") == 0) {
            packet->mode = tftp_MAIL;
        } else {
            err_log("invalid transfer mode: %s", mode);
            ec_set(ec, tftp_EINVALID_MODE);
            return -1;
        }
        break;
    case tftp_DATA:
        packet->data_block_no = extract_number(buf, &pos);
        memcpy(packet->data, buf + pos, buf_len - pos);
        packet->data_size = buf_len - pos;
        pos = buf_len;
        break;
    case tftp_ACK:
        packet->ack_block_no = extract_number(buf, &pos);
        break;
    case tftp_ERROR:
        packet->error_code = extract_number(buf, &pos);
        extract_string(buf, &pos, packet->error_msg);
        break;
    default:
        err_log("invalid opcode: %d", packet->opcode);
        ec_set(ec, tftp_EINVALID_PACKET);
        return -1;
    }

    assert(pos == buf_len);
    return 0;
}

int
make_rrq_or_wrq(tftp_opcode rrq_or_wrq, char const *filename,
                struct tftp_packet *packet, tftp_ec *ec) {
    assert(rrq_or_wrq == tftp_RRQ || rrq_or_wrq == tftp_WRQ);
    *packet = (struct tftp_packet){.opcode = rrq_or_wrq, .mode = tftp_OCTET};
    if (strlcpy(packet->filename, filename, sizeof(packet->filename)) >=
        sizeof(packet->filename)) {
        err_log("filename '%s' is too long to fit into a packet", filename);
        ec_set(ec, tftp_EPACKET_TOO_LONG);
        return -1;
    }
    return 0;
}

void
make_data(unsigned block_no, char buf[512], size_t buf_size,
          struct tftp_packet *packet) {
    *packet = (struct tftp_packet){
        .opcode = tftp_DATA, .data_block_no = block_no, .data_size = buf_size};
    memcpy(packet->data, buf, buf_size);
}

void
make_ack(unsigned block_no, struct tftp_packet *packet) {
    *packet =
        (struct tftp_packet){.opcode = tftp_ACK, .ack_block_no = block_no};
}

int
make_error(tftp_packet_error error_code, char const *error_msg, tftp_ec *ec,
           struct tftp_packet *packet) {
    *packet =
        (struct tftp_packet){.opcode = tftp_ERROR, .error_code = error_code};
    if (strlcpy(packet->error_msg, error_msg, sizeof(packet->error_msg)) >=
        sizeof(packet->error_msg)) {
        err_log("error message '%s' is too long to fit into a packet",
                error_msg);
        ec_set(ec, tftp_EPACKET_TOO_LONG);
        return -1;
    }
    return 0;
}
