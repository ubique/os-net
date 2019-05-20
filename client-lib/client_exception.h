//
// Created by Михаил Терентьев on 2019-05-14.
//

#ifndef OS_NET_CLIENT_EXCEPTION_H
#define OS_NET_CLIENT_EXCEPTION_H


#include <exception>

struct client_exception : runtime_error {
    explicit client_exception(string const &message) : runtime_error("Client exception: " + message + '\n') {};
};


#endif //OS_NET_CLIENT_EXCEPTION_H
