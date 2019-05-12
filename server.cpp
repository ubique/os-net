#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <stdlib.h>
#include <string.h>
#include <string>
#include <chrono>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <system_error>

#include "server.h"
#include "protocol.h"

NTPServer::NTPServer(uint16_t port)
{
    struct addrinfo hints = {};
    hints.ai_family    = AF_INET6;    // Explicitly specify IPv6
                                      // By doing so and relying on IPV6_V6ONLY,
                                      // we are able to accept both IPv4 & IPv6
    hints.ai_socktype  = SOCK_DGRAM;  // Datagram socket
    hints.ai_flags     = AI_PASSIVE;  // For wildcard IP address
    hints.ai_protocol  = IPPROTO_UDP;
    hints.ai_canonname = nullptr;
    hints.ai_addr      = nullptr;
    hints.ai_next      = nullptr;

    struct addrinfo* server_addr;
    int s = getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &server_addr);
    if (s != 0) {
        throw std::runtime_error(std::string("getaddrinfo failed: ") + gai_strerror(s));
    }

    // getaddrinfo() returns a list of address structures
    // Trying each until we successfully bind
    struct addrinfo* rp;
    for (rp = server_addr; rp != nullptr; rp = rp->ai_next) {
        m_sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        // Attempting to disable IPV6_V6ONLY; if this fails, we can still work
        // with IPv6 clients. Depends on OS: on Linux is default, not on BSDs.
        const int NO = 0;
        if (setsockopt(m_sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &NO, sizeof(NO)) == -1) {
            std::cerr << "Failed to disable IPV6_V6ONLY: " << strerror(errno) << std::endl;
        }
        if (m_sockfd != -1) {
            if (bind(m_sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
                break;  // Success
            }
            close(m_sockfd);
            m_sockfd = -1;
        }
    }
    freeaddrinfo(server_addr);

    // All addresses failed
    if (rp == nullptr) {
        std::error_code ec(errno, std::system_category());
        throw std::system_error(ec, "Failed to bind the socket");
    }
}

NTPServer::~NTPServer()
{
    if (close(m_sockfd) == -1) {
        std::cerr << "Failed to close the socket: " << strerror(errno) << std::endl;
    }
}

[[noreturn]] void NTPServer::run() const
{
    for (;;) {
        ntp_packet packet = {};
        struct sockaddr_storage peer_addr = {};
        socklen_t peer_addr_len = sizeof(struct sockaddr_storage);

        // Receive client request (blocking)
        ssize_t ntransferred = recvfrom(m_sockfd, &packet, sizeof(packet), 0,
                                        reinterpret_cast<struct sockaddr *>(&peer_addr),
                                        &peer_addr_len);
        if (ntransferred != sizeof (packet)) {
            continue;   // ignore failed request
        }

        log_connection(peer_addr);
        fill_packet(packet);

        // Send resulting packet
        ntransferred = sendto(m_sockfd, &packet, sizeof (packet), 0,
                              reinterpret_cast<struct sockaddr *>(&peer_addr),
                              peer_addr_len);
        if (ntransferred != sizeof (packet)) {
            if (ntransferred == -1) {
                std::cerr << "Error sending response: " << strerror(errno) << std::endl;
            } else {
                std::cerr << "Partial data sent" << std::endl;
            }
        }
    }
}

time_t NTPServer::current_ntp_time()
{
    auto sys_time = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(sys_time) + NTP_TIMESTAMP_DELTA;
}

void NTPServer::log_connection(const sockaddr_storage &peer_addr)
{
    char host[NI_MAXHOST], service[NI_MAXSERV];
    int s = getnameinfo(reinterpret_cast<const struct sockaddr *>(&peer_addr),
                    sizeof(struct sockaddr_storage), host, NI_MAXHOST,
                    service, NI_MAXSERV, NI_NUMERICSERV);
    if (s == 0) {
        std::cout << "Connection from " << host << ":" << service << std::endl;
    } else {
        std::cerr << "getaddrinfo failed: " << gai_strerror(s) << std::endl;
    }
}

void NTPServer::fill_packet(ntp_packet &packet)
{
    packet.li_vn_mode = 0b11'011'100; // li = 3 (unknown), vn = 3 (version), mode = 3 (server)
    packet.stratum    = 0b00001111;   // class 15; embracing our imperfections.
    // Only setting seconds from a system clock
    packet.txTm_s = htonl(static_cast<uint32_t>(NTPServer::current_ntp_time()));
    packet.txTm_f = 0;
}

int main(int argc, char* argv[])
{
    if (argc > 2) {
        std::cerr << "Usage: ntp-server [<port number>]" << std::endl;
        return EXIT_FAILURE;
    }
    uint16_t server_port = NTP_DEFAULT_PORT;
    if (argc == 2) {
        if (!(std::istringstream(argv[1]) >> server_port)) {
            std::cerr << "Invalid port number provided" << std::endl;
            return EXIT_FAILURE;
        }
    }

    try {
        NTPServer server(server_port);
        server.run();
    } catch (std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
