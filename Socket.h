//
// Created by max on 07.05.19.
//

#pragma once

#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <memory>

class Socket {
public:
    Socket();

    void create(std::string name, int flag);

    void bind();

    void listen();

    void connect();

    int accept();

    std::pair<ssize_t, std::unique_ptr<char[]>> read();

    void write(const std::string &data);

    void close();

    ~Socket();

public:
    static const int FLAG_SERVER = true;
    static const int FLAG_CLIENT = false;

private:
    int m_flag{};
    struct sockaddr_un addr{};
    int connection_socket{};
    int data_socket{};
    int prev_data_socket = -1;
    std::string soc_name;

};


