#include "debug_echo_client.h"

#include "datagram_socket.h"

#include <vector>
#include <iterator>

void debug_echo_client::run(inet_address inet_addr, uint16_t port, const char* msg, size_t length, uint16_t requests, std::ostream& log_ostream)
{
	datagram_socket socket;
	socket.set_so_timeout(1000);
	std::vector<char> buffer(length);
	for (auto i = requests; i--; )
	{
		log_ostream << "Processing request " << requests - i << "..." << std::endl;
		try {
			datagram_packet packet{ buffer.data(), buffer.size(), 0, inet_addr, port };
			buffer.assign(msg, msg + length);
			log_ostream << "Sending..." << std::endl;
			socket.send(packet);
			log_ostream << "Sent: \n";
			std::copy(packet.data + packet.offset, packet.data + packet.offset + packet.length, std::ostream_iterator<char>(log_ostream));
			log_ostream << "\nWaiting for an answer..." << std::endl;
			socket.receive(packet);
			log_ostream << "Received answer: \n";
			std::copy(packet.data + packet.offset, packet.data + packet.offset + packet.length, std::ostream_iterator<char>(log_ostream));
			log_ostream << std::endl;
		} catch (const datagram_socket::socket_error& ex)
		{
			log_ostream << "Error: " << ex.what() << std::endl;
		}
	}
}