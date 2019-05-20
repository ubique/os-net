//
// Created by Noname Untitled on 20.05.19.
//

#pragma once

class Client {
public:
    explicit Client(const char *, const char *, const std::string &);

    void createConnection();

    void run();

    char* getResponse() const;

private:
    const char *mAddress;
    const char *mPort;
    int serverSocketDescr;

    addrinfo serverHint;
    addrinfo *result;

    addrinfo *serverAddrInfo;

    std::string message;
    char *response;

    void findSuitableAddrs();

};