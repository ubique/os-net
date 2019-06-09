#include "datagram_socket.h"

#include <cstring>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace
{
	template<typename T>
	T check(T arg)
	{
		if (arg == -1)
		{
			throw datagram_socket::socket_error(std::strerror(errno));
		}
		return arg;
	}
}

datagram_socket::datagram_socket() : sfd_(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))
{
	check(sfd_);
}

datagram_socket::~datagram_socket()
{
	close(sfd_);
}

void datagram_socket::bind(uint32_t inet_addr, uint16_t port)
{
	const auto addr = sockaddr_in{ AF_INET, port, in_addr{ inet_addr } };
	check(::bind(sfd_, reinterpret_cast<const sockaddr*>(&addr), sizeof(decltype(addr))));
}

void datagram_socket::receive(datagram_packet& packet)
{
	auto peer_addr = sockaddr_in{};
	socklen_t peer_addr_len = sizeof(decltype(peer_addr));
	const auto nread = check(recvfrom(sfd_, packet.data + packet.offset, packet.length, 0, reinterpret_cast<sockaddr*>(&peer_addr), &peer_addr_len));
	packet.length = nread;
	packet.inet_addr = inet_address(peer_addr.sin_addr.s_addr);
	packet.port = peer_addr.sin_port;
}

void datagram_socket::send(const datagram_packet& packet)
{
	auto peer_addr = sockaddr_in{ AF_INET, packet.port, packet.inet_addr->get_address() };
	const socklen_t peer_addr_len = sizeof(decltype(peer_addr));
	check(sendto(sfd_, packet.data + packet.offset, packet.length, 0, reinterpret_cast<sockaddr*>(&peer_addr), peer_addr_len));
}

void datagram_socket::set_so_timeout(uint16_t millis)
{
	auto timeout = timeval{ 0, millis * 100 };
	check(setsockopt(sfd_, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout)));
	check(setsockopt(sfd_, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout)));
}