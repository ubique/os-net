all: client server

client: client.o
	g++ -o client client.o

server: server.o
	g++ -o server server.o

client.o: client.cpp
	g++ -c client.cpp	

server.o: server.cpp
	g++ -c server.cpp

clean:
	rm -rf *.o *.so *.a main
	
