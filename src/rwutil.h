#ifndef SFTP_SERVER_RWUTIL
#define SFTP_SERVER_RWUTIL

#include <string>

std::string read_message(int socket_fd) noexcept(false);

void write_message(int socket_fd, std::string const &message) noexcept(false);

#endif
