//
// Created by Михаил Терентьев on 2019-06-08.
//

#ifndef OS_NET_DESCRIPTOR_PASSING_WRAPPER_H
#define OS_NET_DESCRIPTOR_PASSING_WRAPPER_H


#include <unistd.h>
#include <iostream>

struct wrapper {
    wrapper();

    wrapper(int const &fd);

    wrapper(wrapper const &) = delete;

    wrapper &operator=(wrapper const &) = delete;

    wrapper &operator=(wrapper &&other)noexcept ;

    bool isBroken();

    int getDescriptor();

    ~wrapper();

private:
    int descriptor;
};


#endif //OS_NET_DESCRIPTOR_PASSING_WRAPPER_H
