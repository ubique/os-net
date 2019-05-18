#ifndef TFTP_TFTP_EC_H
#define TFTP_TFTP_EC_H

#define ERROR_CODES(X)                                                         \
    X(tftp_EAGAIN,                                                             \
      "socket receive timeout expired before any data was received")           \
    X(tftp_EBIND, "bind failed")                                               \
    X(tftp_ECANT_RESOLVE_HOST, "getnameinfo failed")                           \
    X(tftp_ECONNECT, "connect failed")                                         \
    X(tftp_EERROR_PACKET_RECEIVED, "received error packet in response")        \
    X(tftp_EFOPEN, "fopen failed")                                             \
    X(tftp_EGAI, "getaddrinfo failed")                                         \
    X(tftp_EINVALID_MODE, "Transfer mode of received packet was not one of "   \
                          "'netascii', 'octet', 'mail'")                       \
    X(tftp_EINVALID_PACKET, "Received packet was somehow corrupted")           \
    X(tftp_EIO_PROBLEM, "IO operation failed")                                 \
    X(tftp_EMODE_NOT_SUPPORTED, "Transfer mode of received packet is "         \
                                "unsupported (probably, not 'octet')")         \
    X(tftp_ENOMEM, "Failed to allocate memory")                                \
    X(tftp_ENO_REPLY, "Attempted to receive packet several times, but failed") \
    X(tftp_EPACKET_LOST,                                                       \
      "Attempted to send packet several times, but it keeps getting lost")     \
    X(tftp_EPACKET_TOO_LONG, "Data is too big to fit into packet")             \
    X(tftp_ERECV, "recvfrom failed")                                           \
    X(tftp_ESEND, "sendto failed")                                             \
    X(tftp_ESETSOCKOPT, "setsockopt failed")                                   \
    X(tftp_ESOCKET, "socket failed")

#define ERROR_ENUM(NAME, TEXT) NAME,
#define ERROR_TEXT(NAME, TEXT)                                                 \
    case NAME:                                                                 \
        return TEXT;

typedef enum { ERROR_CODES(ERROR_ENUM) } tftp_ec;

static inline const char *
tftp_strerror(tftp_ec ec) {
    switch (ec) {
        ERROR_CODES(ERROR_TEXT)
    default:
        return "unknown error";
    }
}

#undef ERROR_CODES
#undef ERROR_ENUM
#undef ERROR_TEXT

#endif // TFTP_TFTP_EC_H
