#ifndef DATAGRAM_SOCKET_H
#define DATAGRAM_SOCKET_H

#include <stdexcept>

#include "datagram_packet.h"

struct datagram_socket
{
	datagram_socket();
	~datagram_socket();

	datagram_socket(const datagram_socket& other) = delete;
	datagram_socket& operator=(const datagram_socket& other) = delete;

	void bind(uint32_t inet_addr, uint16_t port);

	void receive(datagram_packet& packet);
	void send(const datagram_packet& packet);

	void set_so_timeout(uint16_t millis);

	struct socket_error : std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};

private:
	int sfd_;
};

#endif // DATAGRAM_SOCKET_H