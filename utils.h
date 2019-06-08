#ifndef OS_NET_UTILS_H
#define OS_NET_UTILS_H

#include <iostream>
#include <vector>
#include <sys/socket.h>

using std::string;

class utils {
public:
    static void check(int var_to_check, const char* msg);
    static void receive_msg(char *buf_ptr, int size, int sender);
    static void send_msg(char *buf_ptr, int size, int receiver);
    static void print_msg(const string& msg);
};


#endif //OS_NET_UTILS_H
