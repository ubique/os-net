#pragma once
#include<cstdint>

class socket_handler {
    int descriptor;
public:
    socket_handler();
    ~socket_handler();
    void bind(uint32_t addr, uint16_t port);
    int get_descriptor();
};

