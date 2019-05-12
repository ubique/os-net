#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <system_error>

#include "client.h"
#include "protocol.h"

NTPClient::NTPClient(const std::string &hostname, uint16_t port)
 : m_server_addr()
{
    struct addrinfo hints = {};
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = AF_UNSPEC;    // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM;  // Datagram socket
    hints.ai_flags    = 0;
    hints.ai_protocol = IPPROTO_UDP;

    int s = getaddrinfo(hostname.c_str(), std::to_string(port).c_str(), &hints, &m_server_addr);
    if (s != 0) {
        throw std::runtime_error(std::string("getaddrinfo failed: ") + gai_strerror(s));
    }
}

NTPClient::~NTPClient()
{
    freeaddrinfo(m_server_addr);
}

time_t NTPClient::get_time() const
{
    const int sockfd = open_socket();

    ntp_packet packet = {};
    packet.li_vn_mode = 0b11'011'011; // li = 3 (unknown), vn = 3 (version), mode = 3 (client)

    // Send a request
    if (write(sockfd, &packet, sizeof(ntp_packet)) == -1) {
        close(sockfd);
        std::error_code ec(errno, std::system_category());
        throw std::system_error(ec, "Write failed");
    }

    // Wait for the response (blocking)
    if (read(sockfd, &packet, sizeof(ntp_packet)) == -1) {
        close(sockfd);
        std::error_code ec(errno, std::system_category());
        throw std::system_error(ec, "Read failed");
    }

    if (close(sockfd) == -1) {
        std::error_code ec(errno, std::system_category());
        throw std::system_error(ec, "Failed to close the socket");
    }

    // Using time-stamp seconds: time when the packet left the NTP server,
    // in seconds passed since 1900
    // ntohl() converts the bit/byte order from the network's to host's "endianness"

    packet.txTm_s = ntohl(packet.txTm_s); // seconds
    packet.txTm_f = ntohl(packet.txTm_f); // fractions of a second

    // Subtract 70 years from the NTP epoch time to get UNIX epoch
    // (seconds passed since 1970)
    return static_cast<time_t>(packet.txTm_s - NTP_TIMESTAMP_DELTA);
}

int NTPClient::open_socket() const
{
    // getaddrinfo() returns a list of address structures
    // Trying each until we successfully connect
    int sockfd = -1;
    struct addrinfo* rp;
    for (rp = m_server_addr; rp != nullptr; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd != -1) {
            if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1) {
                break;  // Success
            }

            close(sockfd);
            sockfd = -1;
        }
    }

    // All addresses failed
    if (rp == nullptr) {
        std::error_code ec(errno, std::system_category());
        throw std::system_error(ec, "Failed to open and connect a socket");
    }

    return sockfd;
}

int main(int argc, char* argv[])
{
    std::string server_hostname = NTP_DEFAULT_SERVER;
    uint16_t server_port = NTP_DEFAULT_PORT;
    if (argc > 3) {
        std::cerr << "Usage: ntp-client [<server hostname or address> [<server port>]]" << std::endl;
        return EXIT_FAILURE;
    }

    if (argc >= 2) {
        server_hostname = argv[1];
    }

    if (argc == 3) {
        if (!(std::istringstream(argv[2]) >> server_port)) {
            std::cerr << "Invalid port number provided" << std::endl;
            return EXIT_FAILURE;
        }
    }

    try {
        NTPClient client(server_hostname, server_port);
        time_t time = client.get_time();
        std::cout << "Time: " << ctime(&time);
    } catch (std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
