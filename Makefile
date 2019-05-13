all: buildAll

buildServer: server.cpp
	g++ -o server server.cpp

buildClient: client.cpp
	g++ -o client client.cpp

buildAll: buildServer buildClient

runServer: buildServer
	./server 127.0.0.1 8080

runClient: buildClient
	./client 127.0.0.1 8080 Hello\ world\!

clean:
	rm -rf client server
