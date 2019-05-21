#include <iostream>
#include "POP3Server.h"
#include "Utils.h"
#include "DBase.h"

POP3Server::POP3Server(const std::string &host_name, int port = 110) {
    struct hostent *server;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        print_error("ERROR opening socket");
    }

    server = gethostbyname(host_name.c_str());
    if (server == nullptr) {
        print_error("ERROR, no such host");
        stop();
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(reinterpret_cast<char*>(&server_addr.sin_addr.s_addr), static_cast<char*>(server->h_addr), server->h_length);
    server_addr.sin_port = htons(port);

    if (bind(socket_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
        print_error("ERROR bind a socket");
        stop();
    }

    if (listen(socket_fd, 16) == -1) {
        print_error("ERROR listen socket");
        stop();
    }
}

void POP3Server::print_error(const std::string &msg) {
    perror(msg.c_str());
}


int POP3Server::run() {
    DBase data_base;
    while (true) {
        States state = AUTHORIZATION;
        User user;
        sockaddr_in client{};
        socklen_t client_len;
        int client_fd = accept(socket_fd, reinterpret_cast<sockaddr *>(&client), &client_len);
        if (client_fd == -1) {
            print_error("Can't accept a client!\n");
            continue;
        } else {
            send_msg("+OK POP3 server ready", client_fd, "Error in send hello to client!");
        }
        while (true) {
            if (state == UPDATE) {
                send_msg("+OK, all messages updated", client_fd, "Error in updating");
                data_base.update(user.get_login());
                close(client_fd);
                break;
            }
            std::cout << "state: " << state << std::endl;
            char buffer[BUFFER_LENGHT];
            memset(buffer, 0, BUFFER_LENGHT);
            size_t msg_len = recv(client_fd, &buffer, BUFFER_LENGHT, 0);
            if (msg_len == -1) {
                print_error("Error to receive a message");
                continue;
            }
            std::vector<std::string> request = Utils::split(buffer);
            if (request.empty() || request.size() > 2) {
                continue;
            }
            std::cout << request.size() << std::endl;
            if (request.size() == 1) {
                std::cout << request[0] << std::endl;
            } else {
                std::cout << request[0] << " " << request[1] << std::endl;
            }
            if (state == AUTHORIZATION) {
                if (request[0] == "USER") {
                    if (request.size() == 2) {
                        if (data_base.is_user_by_login(request[1])) {
                            send_msg("+OK, user was found", client_fd, "Error in sending");
                            user = data_base.get_user_by_login(request[1]);
                        } else {
                            send_msg("-ERR, can't find user", client_fd, "Error in sending");
                        }
                    } else {
                        send_msg("-ERR no login", client_fd, "Error in sending");
                    }
                } else if (request[0] == "PASS") {
                    if (request.size() == 2) {
                        if (user.get_login().empty()) {
                            send_msg("-ERR, haven't user session", client_fd, "Error in sending");
                        } else if (user.cmp_password(request[1])) {
                            send_msg("+OK, authorization success", client_fd, "Error in sending");
                            state = TRANSACTION;
                        } else {
                            send_msg("-ERR, password is incorrect", client_fd, "Error in sending");
                        }
                    } else {
                        send_msg("-ERR no password", client_fd, "Error in sending");
                    }
                } else if (request[0] == "QUIT") {
                    send_msg("+OK, session closed", client_fd, "Error in sending");
                    break;
                } else {
                    send_msg("-ERR, unknown command. You must authorization", client_fd, "Error in sending");
                }
            } else {
                if (request[0] == "QUIT") {
                    state = UPDATE;
                } else if (request[0] == "STAT") {
                    auto messages = data_base.get_messages_by_login(user.get_login());
                    std::string response = "+OK " + std::to_string(messages.size()) + " " +
                                           std::to_string(get_size_of_vector(messages));
                    send_msg(response, client_fd, "Error in sending");
                } else if (request[0] == "LIST") {
                    auto messages = data_base.get_messages_by_login(user.get_login());
                    std::string response;
                    if (messages.empty()) {
                        response = "-ERR, no such message";
                    } else {
                        response = "+OK " + std::to_string(messages.size()) + " messages (" +
                                   std::to_string(get_size_of_vector(messages)) + "bytes)" + CRLF;
                        if (request.size() == 2) {
                            size_t ind = std::stoi(request[1]);
                            if (ind <= 0 || ind > messages.size()) {
                                response = "-ERR, incorrect number of messages";
                            } else {
                                response += request[1] + " " + std::to_string(messages[ind - 1].get_size());
                            }
                        } else {
                            for (size_t i = 0; i < messages.size(); i++) {
                                response += std::to_string(i + 1) + " " + std::to_string(messages[i].get_size()) +
                                            (messages.size() - 1 == i ? "" : CRLF);
                            }
                        }
                    }
                    send_msg(response, client_fd, "Error in sending");
                } else if (request[0] == "RETR") {
                    if (request.size() == 2) {
                        auto messages = data_base.get_messages_by_login(user.get_login());
                        std::string response;
                        if (messages.empty()) {
                            response = "-ERR no such elements";
                        } else {
                            size_t ind = std::stoi(request[1]);
                            if (ind <= 0 || messages.size() < ind) {
                                response = "-ERR incorrect format of number";
                            } else {
                                response = "+OK " + std::to_string(messages[ind - 1].get_size()) + " bytes" + CRLF;
                                response += messages[ind - 1].get_text() + CRLF;
                            }
                        }
                        send_msg(response, client_fd, "Error in sending");
                    } else {
                        send_msg("-ERR", client_fd, "Error in sending");
                    }
                } else if (request[0] == "DELE") {
                    if (request.size() == 2) {
                        auto messages = data_base.get_messages_by_login(user.get_login());
                        size_t ind = std::stoi(request[1]);
                        std::string response;
                        if (ind <= 0 || messages.size() < ind) {
                            response = "-ERR incorrect format of number";
                        } else {
                            if (messages[ind - 1].is_removed()) {
                                response = "-ERR alredy deleted";
                            } else {
                                data_base.remove_msg(user.get_login(), ind - 1);
                                response = "+OK message deleted";
                            }
                        }
                        send_msg(response, client_fd, "Error in sending");
                    } else {
                        send_msg("-ERR haven't number", client_fd, "Error in sending");
                    }
                } else if (request[0] == "NOOP") {
                    send_msg("+OK", client_fd, "Error in sending");
                } else if (request[0] == "RSET") {
                    data_base.unremove_msgs(user.get_login());
                    send_msg("+OK", client_fd, "Error in sending");
                } else {
                    send_msg("-ERR, unknown command", client_fd, "Error in sending");
                }
            }
        }
    }
}

POP3Server::~POP3Server() {
    stop();
}

void POP3Server::send_msg(const std::string& msg, int fd, const std::string& msg_error) {
    if (send(fd, (msg + CRLF).c_str(), msg.size() + 1, 0) == -1) {
        print_error(msg_error);
        close(fd);
    }
}

size_t POP3Server::get_size_of_vector(std::vector<Message> &messages) {
    size_t res = 0;
    for(Message msg : messages) {
        res += msg.get_size();
    }
    return res;
}

void POP3Server::stop() {
    if (close(socket_fd) == -1) {
        print_error("Can't stop a server\n");
    }
    // some operations
}
