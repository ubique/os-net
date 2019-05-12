#ifndef NTP_SERVER_H
#define NTP_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>

#include <stdint.h>
#include <time.h>

#include "protocol.h"

class NTPServer
{
public:
    explicit NTPServer(uint16_t port);
    ~NTPServer();
    [[noreturn]] void run() const;
private:
    static time_t current_ntp_time();
    static void log_connection(const struct sockaddr_storage& peer_addr);
    static void fill_packet(ntp_packet& packet);

    int m_sockfd;
};

#endif // NTP_SERVER_H
