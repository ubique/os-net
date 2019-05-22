//
// Created by Noname Untitled on 20.05.19.
//

#pragma once

class Client {
public:
    explicit Client(const char *, const char *, std::string );

    ~Client();

    void createConnection();

    void sendInfo();

    char* getResponse() const;

private:
    int mClientSocket;
    int mPort;

    struct sockaddr_in mSocketAddress;

    std::string message;
    char *response;

};