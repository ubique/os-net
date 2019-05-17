all: server client

server: server.cpp
	g++ -std=c++14 $^ -o $@
client: client.cpp
	g++ -std=c++14 $^ -o $@
clean:
	rm -rf client server
