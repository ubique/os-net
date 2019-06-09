#ifndef ECHO_SERVER_H
#define ECHO_SERVER_H

#include <cstdint>

#include "inet_address.h"

struct echo_server
{
	void start(inet_address inet_addr, uint16_t port);
};

#endif // ECHO_SERVER_H