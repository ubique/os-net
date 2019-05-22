//
// Created by Noname Untitled on 19.05.19.
//

#pragma once

class Server {
public:
    explicit Server(const char *);

    ~Server();

    void createBinding();

    void run();

private:
    int mServerSocket;
    int mPort;

    struct sockaddr_in mSocketAddress;

    Database mDatabase;

    std::string processGivenCommand(const char *);
};