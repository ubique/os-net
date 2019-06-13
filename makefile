CXX = clang++
CXX_STANDARD = -std=c++17

all: buildAll

buildServer: server.cpp
	$(CXX) $(CXX_STANDARD) -o server server.cpp

buildClient: client.cpp
	$(CXX) $(CXX_STANDARD) -o client client.cpp

buildAll: buildServer buildClient

runServer: buildServer
	./server 127.0.0.1:8080

runClient: buildClient
	./client 127.0.0.1:8080 test

clean:
	rm -rf client server