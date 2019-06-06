#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <Utils.h>

void print_error(const std::string &msg) {
    perror(msg.c_str());
    exit(EXIT_FAILURE);
}

int send_msg(const std::string& msg, int fd, const std::string& msg_error) {
    int len = msg.size() + 1;
    int sended = 0;
    int ost = len;
    char buffer[len];
    strcpy(buffer, msg.c_str());
    while(sended < len) {
        int count = send(fd, buffer + sended, ost, 0);
        if (count == -1 && errno == EINTR) {
            continue;
        }else if (count == -1){
            close(fd);
            return -1;
        }
        sended += count;
        ost -= count;
    }
    return 0;
}

int read(int fd, std::string &msg) {
    const int BUFFER_LENGHT = 1024;
    char buffer[BUFFER_LENGHT];
    memset(buffer, 0, BUFFER_LENGHT);
    int count = 0;
    int trys = 10;
    while((count = recv(fd, &buffer, BUFFER_LENGHT, 0))) {
        if (count == -1) {
            for (int i = 0; i < trys; i++) {
                if (count != -1 || errno != EINTR) {
                    break;
                }
                count = recv(fd, buffer, BUFFER_LENGHT, 0);
            }
        }
        if (count == -1) {
            print_error("Can't recv from client");
            break;
        }
        if (count == 0) {
            break;
        }
        for(int i = 0; i < count - 1; i++) {
            msg.push_back(buffer[i]);
        }
        if (count < BUFFER_LENGHT) {
            break;
        }
    }
    return 0;
}

int main(int argc, char** argv) {
    if (argc == 0 || argc > 2) {
        std::cerr << "Incorrect count of arguments" << std::endl;
    }
    const char* host_name = argc == 1 ? argv[0] : argv[1];
    struct hostent *server;
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd < 0) {
        print_error("ERROR opening socket");
        return 0;
    }

    server = gethostbyname(host_name);
    if (server == nullptr) {
        print_error("ERROR, no such host");
        close(socket_fd);
        return 0;
    }

    struct sockaddr_in server_addr{};
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(reinterpret_cast<char*>(&server_addr.sin_addr.s_addr), static_cast<char*>(server->h_addr), server->h_length);
    server_addr.sin_port = htons(8888);

    if (connect(socket_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
        print_error("ERROR connect a socket");
        close(socket_fd);
        return 0;
    }
    std::cout << "Client started!" << std::endl;
    std::cout << "For exit print \"exit\"" << std::endl;
    std::string buf;
    read(socket_fd, buf);
    std::cout << buf << std::endl;
    while(true) {
        buf = "";
        std::cout << "Pleas, print a command:" << std::endl;
        std::string command;
        std::getline(std::cin , command);
        if (command == "exit") {
            close(socket_fd);
            exit(EXIT_SUCCESS);
        }
        if(send_msg(command, socket_fd, "Can't send a msg") == -1) {
            print_error("bad!");
            close(socket_fd);
        }
        if (read(socket_fd, buf) == -1) {
            print_error("Can't read!");
        }
        std::cout << "Response from server: " << buf << std::endl;
    }
}