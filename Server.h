//
// Created by Павел Пономарев on 2019-05-19.
//

#ifndef OS_NET_SERVER_H
#define OS_NET_SERVER_H

#include "Utils.h"
#include <string>

class Server {
public:
    Server() = default;
    bool run(std::string const&);
private:
    int openSocket();
    bool bindSocket(int, int);
    bool process(int);

    const int BUFFER_SIZE = 1024;
};


#endif //OS_NET_SERVER_H
