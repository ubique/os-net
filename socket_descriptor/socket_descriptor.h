//
// Created by Anton Shelepov on 2019-05-17.
//

#ifndef OS_NET_SOCKET_DESCRIPTOR_H
#define OS_NET_SOCKET_DESCRIPTOR_H


struct socket_descriptor {
    socket_descriptor();
    socket_descriptor(int descriptor);
    bool valid() const;
    int operator*() const;
    ~socket_descriptor();

private:
    int descriptor = -1;
};


#endif //OS_NET_SOCKET_DESCRIPTOR_H
