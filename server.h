#pragma once

#include "util.hpp"

#include <signal.h>

struct socks5_server final
{
    socks5_server(socks5_server const&) = delete;

    socks5_server(uint16_t port,
                  sig_atomic_t volatile& termination_marker);

    void run() noexcept;

private:
    uint16_t const port;
    sig_atomic_t volatile& termination_marker;
};
