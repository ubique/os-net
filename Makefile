CXX = g++
CXX_STANDARD = -std=c++14

all: build

build: client.cpp server.cpp
	$(CXX) $(CXX_STANDARD) -o client client.cpp
	$(CXX) $(CXX_STANDARD) -o server server.cpp

run_client: client
	./client

run_server: server
	./server

.PHONY: clean
clean:
	rm client
	rm server