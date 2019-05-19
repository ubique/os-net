//
// Created by Павел Пономарев on 2019-05-19.
//

#ifndef OS_NET_CLIENT_H
#define OS_NET_CLIENT_H

#include "Utils.h"
#include <string>

class Client {
public:
    Client() = default;
    bool run(std::string const&);
private:
    int openSocket();
    bool connectSocket(int, int);

    const int BUFFER_SIZE = 1024;

};


#endif //OS_NET_CLIENT_H
