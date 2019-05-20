//
// Created by rsbat on 5/12/19.
//

#include "POP_server.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

using std::cerr;
using std::endl;


std::string get_error_message(const std::string &comment) {
    return comment + ": " + strerror(errno);
}

POP_server::POP_server(std::string address, std::string port) : state(State::WAIT), socket_fd(), current_user(), mail_db() {
    char* working_dir = get_current_dir_name();
    if (working_dir == nullptr) {
        throw std::runtime_error(get_error_message("Could not get name of current directory"));
    }
    mail_db.set_mail_dir(working_dir);
    free(working_dir);

    socket_fd.set(socket(AF_INET, SOCK_STREAM, 0));
    if (socket_fd == -1) {
        throw std::runtime_error(get_error_message("Could not create socket"));
    }

    struct sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(static_cast<uint16_t>(std::stoi(port)));

    int pton_status = inet_pton(AF_INET, address.c_str(), &serv_addr.sin_addr);
    if (pton_status == 0) {
        throw std::invalid_argument(get_error_message(address + " is not a valid address"));
    } else if (pton_status == -1) {
        throw std::domain_error(get_error_message("AF_INET is not supported"));
    }

    if (bind(socket_fd,
            reinterpret_cast<sockaddr*>(&serv_addr),
            sizeof(sockaddr_in)) == -1) {
        throw std::runtime_error(get_error_message("Could not bind socket"));
    }

    if (listen(socket_fd, MAX_BACKLOG) == -1) {
        throw std::runtime_error(get_error_message("Could not listen to socket"));
    }
}

[[noreturn]] void POP_server::run(){
    char buffer[BUFFER_SIZE];

    for(;;) {
        sockaddr_in client{};
        socklen_t sz = sizeof(sockaddr_in);
        client_socket.set(accept(socket_fd, reinterpret_cast<sockaddr*>(&client), &sz));

        if (client_socket == -1) {
            cerr << get_error_message("Could not accept connection") << endl;
            continue;
        }

        state = State::AUTH;
        std::string greeting = "+OK Simple synchronous POP3 server (username: user, password: empty)\r\n";
        write(client_socket, greeting.c_str(), greeting.size());

        while (state != State::WAIT && state != State::UPDATE) {
            memset(buffer, 0, BUFFER_SIZE);
            ssize_t bytes_read = read(client_socket, buffer, BUFFER_SIZE);
            if (bytes_read == -1) {
                cerr << get_error_message("Could not read data from client") << endl;
                state = State::WAIT;
                break;
            } else if (bytes_read == BUFFER_SIZE) {
                send_ERR("Command is too long");
                state = State::WAIT;
            }
            process_command(buffer);
        }

        if (state == State::UPDATE) {
            mail_db.apply_delete();
        }
        mail_db.close_inbox();
        current_user.clear();
        state = State::WAIT;
        
        send_OK("Inbox closed");

        client_socket.close();
    }
}

void POP_server::process_command(std::string command_line) {
    auto pos = command_line.find(' ');
    auto command = command_line.substr(0, pos);
    auto argument = (pos == std::string::npos) ? "" : command_line.substr(pos + 1);
    while (!command.empty() && isspace(command.back())) {
        command.pop_back();
    }
    while (!argument.empty() && isspace(argument.back())) {
        argument.pop_back();
    }

    cerr << command << ":" << argument << endl;

    if (state == State::AUTH) {
        if (command == "user") {
            if (current_user.empty()) {
                current_user = argument;
                send_OK();
            } else {
                current_user.clear();
                send_ERR("User name was already sent");
            }
        } else if (command == "pass") {
            if (current_user.empty()) {
                send_ERR("User name must be sent first");
            } else {
                if (mail_db.open_inbox(current_user)) {
                    state = State::TRANSACTION;
                    send_OK();
                } else {
                    current_user.clear();
                    send_ERR("Could not open inbox");
                }
            }
        } else if (command == "quit") {
            state = State::WAIT;
            send_OK();
        } else {
            send_ERR("Illegal command");
        }
    } else if (state == State::TRANSACTION) {
        if (command == "stat") {
            auto data = mail_db.stat();
            send_OK(std::to_string(data.first) + " " + std::to_string(data.second));
        } else if (command == "list") {
            if (argument.empty()) {
                auto data = mail_db.list();
                std::string response = std::to_string(data.size()) + " messages\r\n";
                for (auto entry : data) {
                    response += std::to_string(entry.first);
                    response += " ";
                    response += std::to_string(entry.second);
                    response += "\r\n";
                }
                response += ".";
                send_OK(response);
            } else {
                try {
                    size_t n = std::stoul(argument);
                    auto data = mail_db.list(n - 1);
                    send_OK(std::to_string(data.first) + " " + std::to_string(data.second));
                } catch (std::logic_error& error) {
                    send_ERR(argument + " is not a valid email number");
                }
            };
        } else if (command == "retr") {
            try {
                size_t n = std::stoul(argument);
                auto data = mail_db.retr(n - 1);
                size_t size = mail_db.list(n - 1).second;
                send_OK(std::to_string(size) + " octets\r\n" + data);
            } catch (std::logic_error& error) {
                send_ERR(argument + " is not a valid email number");
            }
        } else if (command == "dele") {
            try {
                size_t n = std::stoul(argument);
                mail_db.dele(n - 1);
                send_OK();
            } catch (std::logic_error& error) {
                send_ERR(argument + " is not a valid email number");
            }
        } else if (command == "noop") {
            send_OK();
        } else if (command == "rset") {
            mail_db.rset();
            send_OK();
        } else if (command == "quit") {
            state = State::UPDATE;
        } else {
            send_ERR("Illegal command");
        }
    } else {
        throw std::logic_error("When processing commands state must be AUTH or TRANSACTION");
    }
}

void POP_server::send_OK(std::string const& message) {
    std::string response = "+OK " + message + "\r\n";
    send_all(response.c_str(), response.size());
}

void POP_server::send_OK() {
    send_OK("");
}

void POP_server::send_ERR(std::string const& message) {
    std::string response = "-ERR " + message + "\r\n";
    send_all(response.c_str(), response.size());
}

/**
 * Tries to send all data
 * If fails sets state to WAIT
 * @param data
 * @param size
 */
void POP_server::send_all(const char *data, size_t size) {
    auto ptr = data;
    do {
        ssize_t written = write(client_socket, ptr, size);
        if (written == -1) {
            cerr << get_error_message("Could not write to socket") << endl;
            state = State::WAIT;
            return;
        }
        size -= written;
        ptr += written;
    } while (size > 0);
}