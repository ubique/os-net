#include "echo_server.h"

#include "datagram_socket.h"

#include <array>

void echo_server::start(inet_address inet_addr, uint16_t port)
{
	static constexpr size_t buffer_size = 1024 * 4;
	std::array<char, buffer_size> buffer;
	datagram_socket socket;
	socket.bind(inet_addr.get_address(), port);
	while (true)
	{
		datagram_packet packet{};
		packet.data = buffer.data();
		packet.length = buffer.size();
		socket.receive(packet);
		socket.send(packet);
	}
}