#pragma once

#include<string>

void print_error(std::string const& message);
void close_socket(int descriptor);
bool check_port(const char* port);
