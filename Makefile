all: client server

%.o: %.cpp
	g++ -c -o $@ $^

client: EchoClient.o launchClient.o
	g++ EchoClient.o launchClient.o -o client

server: EchoServer.o launchServer.o
	g++ EchoServer.o launchServer.o -o server

clean:
	rm -rf client server *.o
