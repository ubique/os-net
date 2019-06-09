#ifndef INET_ADDRESS_H
#define INET_ADDRESS_H

#include <cstdint>
#include <stdexcept>

#include <arpa/inet.h>

struct inet_address
{
	explicit inet_address(const uint32_t ip)
	{
		inet_addr_ = ip;
	}

	explicit inet_address(const char* const address)
	{
		in_addr addr{};
		if (inet_aton(address, &addr) == 0)
		{
			throw std::invalid_argument("bad address");
		};
		inet_addr_ = addr.s_addr;
	}

	uint32_t get_address() const
	{
		return inet_addr_;
	}

private:
	uint32_t inet_addr_;
};

#endif // INET_ADDRESS_H