GCC := g++ -std=c++14

all: server client


server: server.cpp
	$(GCC) server.cpp -o server

client: client.cpp
	$(GCC) client.cpp -o client

runServer: server
	./server

runClient: client
	./client
	
clean:
	rm server client
