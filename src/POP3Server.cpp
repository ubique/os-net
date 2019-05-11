#include "POP3Server.h"

int POP3Server::run(const std::string &host_name, int port = 110) {
    struct hostent *server;

    socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd < 0) {
        print_error("ERROR opening socket\n");
        return FAIL;
    }

    server = gethostbyname(host_name.c_str());
    if (server == nullptr) {
        print_error("ERROR, no such host\n");
        close(socket_fd);
        return FAIL;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(reinterpret_cast<char*>(&server_addr.sin_addr.s_addr), static_cast<char*>(server->h_addr), server->h_length);
    server_addr.sin_port = htons(port);

    if (bind(socket_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
        print_error("ERROR bind a socket\n");
        close(socket_fd);
        return FAIL;
    }

    if (listen(socket_fd, 16) == -1) {
        print_error("ERROR listen socket\n");
        close(socket_fd);
        return FAIL;
    }

    return SUCCESS;
}

void POP3Server::print_error(const std::string &msg) {
    perror(msg.c_str());
    exit(EXIT_FAILURE);
}

void POP3Server::execute() {
    while(true) {
        int cur_fd = accept(socket_fd, reinterpret_cast<sockaddr*>(&server_addr),reinterpret_cast<socklen_t *>(sizeof(struct sockaddr_in)));
        if (cur_fd == -1) {
            break;
        }
    }
}

