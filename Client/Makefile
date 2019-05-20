all: Client.o client_main.o
	g++ Client.o client_main.o -o client

Client.o: Client.cpp
	g++ -c Client.cpp

client_main.o: client_main.cpp
	g++ -c client_main.cpp

clean:
	rm -rfv *.o client