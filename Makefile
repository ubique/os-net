all: client.cpp server.cpp
	g++ client.cpp utils.cpp -o client
	g++ server.cpp utils.cpp -o server

default_client: 
	./client 127.0.0.1 7

default_server: 
	./server 127.0.0.1 7

clean:
	rm client
	rm server
