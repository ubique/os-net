//
// Created by roman on 04.06.19.
//

#ifndef OS_NET_MY_FD_H
#define OS_NET_MY_FD_H


class my_fd {
public:
    my_fd();
    my_fd(int fd);

    my_fd(const my_fd& other) = delete;
    my_fd& operator=(const my_fd&) = delete;

    operator int() const;
    ~my_fd();
private:
    int fd;
};


#endif //OS_NET_MY_FD_H
