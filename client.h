#ifndef NTP_CLIENT_H
#define NTP_CLIENT_H

#include <netdb.h>

#include <string>
#include <stdint.h>
#include <time.h>

class NTPClient
{
public:
    NTPClient(const std::string& hostname, uint16_t port);
    ~NTPClient();
    time_t get_time() const;
private:
    int open_socket() const;

    struct addrinfo* m_server_addr;  // NTP server address
};

#endif // NTP_CLIENT_H
