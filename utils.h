#include <assert.h> // assert
#include <stdint.h> // uint16_t
#include <stdio.h>  // fprintf, stderr, stdout
#include <stdlib.h> // abort
#include <string.h> // strlen

#include "tftp_peer.h"

#ifndef NDEBUG
#define unreachable(MSG) (assert(0 && MSG), abort())
#else
#define unreachable(MSG)                                                       \
    (std::fprintf(stderr, "UNREACHABLE executed at %s:%d\n", __FILE__,         \
                  __LINE__),                                                   \
     abort())
#endif

#define err_log(fmt, ...) fprintf(stderr, "ERROR: " fmt "\n", ##__VA_ARGS__)
#define info_log(fmt, ...) fprintf(stdout, "INFO: " fmt "\n", ##__VA_ARGS__)

#define ec_set(ec, value)                                                      \
    do {                                                                       \
        if (ec)                                                                \
            *(ec) = (value);                                                   \
    } while (0)

static inline size_t
strlcpy(char *restrict dst, char const *restrict src, size_t size) {
    size_t src_len = strlen(src);
    if (size != 0) {
        while (*src != '\0' && size > 1) {
            *dst++ = *src++;
            --size;
        }
        *dst = '\0';
    }
    return src_len;
}

#define SCOPED_FILE __attribute__((cleanup(cleanup_file)))
#define SCOPED_PEER __attribute__((cleanup(cleanup_peer)))

static inline void
cleanup_file(FILE **file) {
    fclose(*file);
}

static inline void
cleanup_peer(struct tftp_peer **peer) {
    tftp_peer_destroy(*peer);
}
