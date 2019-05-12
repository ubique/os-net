#ifndef NTP_PROTOCOL_H
#define NTP_PROTOCOL_H

#include <stdint.h>

const char NTP_DEFAULT_SERVER[] = "0.pool.ntp.org";
const uint16_t NTP_DEFAULT_PORT = 123;

const uint32_t NTP_TIMESTAMP_DELTA = 2208988800;

// Structure that defines the 48 byte NTP packet
typedef struct
{

uint8_t li_vn_mode;      // 8 bits: li, vn, and mode.
                            // li:   2 bits, leap indicator
                            // vn:   3 bits, version number of the protocol
                            // mode: 3 bits, client will pick mode 3 for client

uint8_t stratum;         // stratum level of the local clock
uint8_t poll;            // maximum interval between successive messages
uint8_t precision;       // precision of the local clock

uint32_t rootDelay;      // total round trip delay time
uint32_t rootDispersion; // max error aloud from primary clock source
uint32_t refId;          // reference clock identifier

uint32_t refTm_s;        // reference time-stamp seconds
uint32_t refTm_f;        // reference time-stamp fraction of a second

uint32_t origTm_s;       // originate time-stamp seconds
uint32_t origTm_f;       // originate time-stamp fraction of a second

uint32_t rxTm_s;         // received time-stamp seconds
uint32_t rxTm_f;         // received time-stamp fraction of a second

uint32_t txTm_s;         // the most important field the client cares about: transmit time-stamp seconds
uint32_t txTm_f;         // transmit time-stamp fraction of a second

} ntp_packet;

#endif // NTP_PROTOCOL_H


