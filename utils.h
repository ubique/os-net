#ifndef OS_NET_UTILS_H
#define OS_NET_UTILS_H


#include <string>

std::pair<std::string, ssize_t > socket_read(int sock, bool throw_flag = false);

void socket_send(int sock, std::string &str, bool throw_flag = false);

#endif //OS_NET_UTILS_H
