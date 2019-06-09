#ifndef DATAGRAM_PACKET_H
#define DATAGRAM_PACKET_H

#include <cstdint>
#include <optional>

#include "inet_address.h"

struct datagram_packet
{
	char* data;
	size_t length;
	size_t offset;
	std::optional<inet_address> inet_addr; 
	uint16_t port;
};

#endif // DATAGRAM_PACKET_H