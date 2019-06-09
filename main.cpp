#include "echo_server.h"
#include "debug_echo_client.h"
#include "inet_address.h"

#include <iostream>
#include <regex>

#include <boost/program_options.hpp>

int main(int argc, char* argv[])
{
	namespace po = boost::program_options;
	auto desc = po::options_description("General options");
	std::string mode;
	desc.add_options()
		("help", "help message")
		("mode", po::value<std::string>(&mode), "mode: 'server' or 'client'")
		;
	std::string address;
	uint16_t port;
	auto server_desc = po::options_description("Server options");
	server_desc.add_options()
		("ip", po::value<std::string>(&address)->default_value("127.0.0.1"), "ip to bind to")
		("port", po::value<uint16_t>(&port)->default_value(80), "port to bind to")
		;
	auto client_desc = po::options_description("Client options");
	std::string message;
	uint16_t requests;
	client_desc.add_options()
		("ip", po::value<std::string>(&address)->default_value("127.0.0.1"), "ip to send requests to")
		("port", po::value<uint16_t>(&port)->default_value(80), "port to send requests to")
		("msg", po::value<std::string>(&message)->default_value("Hello World!"), "message to send")
		("n", po::value<uint16_t>(&requests)->default_value(10), "number of requests to send")
		;
	const auto p = po::positional_options_description().add("mode", 1);
	po::variables_map vm;
	try
	{
		store(po::command_line_parser(argc, argv).options(desc).positional(p).allow_unregistered().run(), vm);
		notify(vm);
	}
	catch (const po::error& ex)
	{
		std::cout << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
	
	if (vm.count("help"))
	{
		desc.add(server_desc).add(client_desc);
		std::cout << desc << std::endl;
		return EXIT_SUCCESS;
	}
	if (vm.count("mode"))
	{
		if (mode == "server")
		{
			desc.add(server_desc);
			try
			{
				store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
				notify(vm);
			}
			catch (const po::error& ex)
			{
				std::cout << ex.what() << std::endl;
				return EXIT_FAILURE;
			}
			try {
				const auto addr = inet_address(address.c_str());
				echo_server server;
				server.start(addr, port);
				return EXIT_SUCCESS;
			} catch (const std::invalid_argument& ex)
			{
				std::cout << ex.what() << std::endl;
				return EXIT_FAILURE;
			}
		}
		if (mode == "client")
		{
			desc.add(client_desc);
			try
			{
				store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
				notify(vm);
			}
			catch (const po::error& ex)
			{
				std::cout << ex.what() << std::endl;
				return EXIT_FAILURE;
			}
			try {
				const auto addr = inet_address(address.c_str());
				debug_echo_client client;
				client.run(addr, port, message.data(), message.length(), requests, std::cout);
				return EXIT_SUCCESS;
			} catch (const std::invalid_argument& ex)
			{
				std::cout << ex.what() << std::endl;
				return EXIT_FAILURE;
			}
		}
	}
	desc.add(server_desc).add(client_desc);
	std::cout << desc << std::endl;
	return EXIT_FAILURE;
}
