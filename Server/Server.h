//
// Created by Noname Untitled on 19.05.19.
//

#pragma once

class Server {
public:
    explicit Server(const char*);

    void createBinding();

    void run();

private:

    const char* mPort;
    int serverSocketDescr;

    addrinfo serverHint;
    addrinfo *result;

    addrinfo *serverAddrInfo;

    Database mDatabase;

    void findSuitableAddrs();

    std::string processGivenCommand(const char *);

};