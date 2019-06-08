#include "utils.h"

void utils::check(int var_to_check, const char *msg) {
    if (var_to_check == -1) {
        std::perror(msg);
        exit(EXIT_FAILURE);
    }
}

void utils::send_msg(char *buf_ptr, int size, int receiver) {
    int i = 0;
    while (i < size) {
        int result = send(receiver, buf_ptr + i, size - i, MSG_NOSIGNAL);
        check(result, "in send");
        i += result;
    }
}

void utils::receive_msg(char *buf_ptr, int size, int sender) {
    int i = 0;
    while (i < size) {
        int result = recv(sender, buf_ptr + i, size - i, MSG_NOSIGNAL);
        check(result, "in receive");
        i += result;
    }
}

void utils::print_msg(const string& msg) {
    std::cout << "Received message:\n";
    std::cout << msg << std::endl;
}
