#ifndef DEBUG_ECHO_CLIENT_H
#define DEBUG_ECHO_CLIENT_H

#include <cstdint>
#include <iosfwd>

#include "inet_address.h"

struct debug_echo_client
{
	void run(inet_address inet_addr, uint16_t port, const char* msg, size_t length, uint16_t requests, std::ostream& log_ostream);
};

#endif // DEBUG_ECHO_CLIENT_H