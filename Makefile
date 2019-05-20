GCC := g++ -std=c++14
Host := 8080
Msg := exit

all: server client


server: server.cpp
	$(GCC) server.cpp -o server

client: client.cpp
	$(GCC) client.cpp -o client

runServer: server
	./server $(Host)

runClient: client
	./client $(Host) $(Msg)

clean:
	rm server client
