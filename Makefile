client: 
	./client

default_server: 
	./server 127.0.0.1 7

server: 
	./server

all: client.cpp server.cpp
	g++ client.cpp -o client
	g++ server.cpp -o server

clean:
	rm client
	rm server
