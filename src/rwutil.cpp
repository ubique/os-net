#include "rwutil.h"
#include <arpa/inet.h>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <string>

// message structure:
// first we send/receive 2 bytes of message length
// then we send/receive message directly

namespace
{
const size_t HEADER_SIZE = 2;
const int BUF_SIZE = 4096;

template <size_t BufferSize>
std::string read_exact(int socket_fd, size_t message_size) noexcept(false)
{
    char buf[BufferSize];
    std::string storage;
    do {
        ssize_t received_amount =
            recv(socket_fd, buf, std::min(BufferSize, message_size), 0);
        if (received_amount == -1) {
            throw std::runtime_error(
                std::string{"Failure while receiving. Disconnect."} +
                strerror(errno));
        }
        if (received_amount == 0) {
            return "";
        }
        storage += std::string{buf, static_cast<size_t>(received_amount)};
    } while (storage.size() != message_size);
    return storage;
}

void write_exact(int socket_fd, std::string const &message) noexcept(false)
{
    char const *buffer = message.data();
    size_t offset = 0;
    do {
        ssize_t sent_amount =
            send(socket_fd, buffer + offset, message.size() - offset, 0);
        if (sent_amount == -1) {
            throw std::runtime_error(
                std::string{"Failure while sending. Disconnect. "} +
                strerror(errno));
        }
        if (sent_amount == 0) {
            throw std::runtime_error("Unexpected end of socket life");
        }
        offset += static_cast<size_t>(sent_amount);
    } while (offset != message.length());
}

} // namespace

std::string read_message(int socket_fd) noexcept(false)
{
    std::string header = read_exact<HEADER_SIZE>(socket_fd, HEADER_SIZE);
    if (header.empty()) {
        return "";
    }
    uint16_t const *message_size =
        reinterpret_cast<uint16_t const *>(header.data());
    return read_exact<BUF_SIZE>(socket_fd, *message_size);
}

void write_message(int socket_fd, std::string const &message) noexcept(false)
{
    static_assert(sizeof(size_t) >= 2, "Expect size_t be at least two bytes");
    size_t message_size = message.size();
    std::string message_size_representation =
        std::string{reinterpret_cast<char *>(&message_size), 2};
    write_exact(socket_fd, message_size_representation);
    write_exact(socket_fd, message);
}
