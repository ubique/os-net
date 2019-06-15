CXX := g++
OPTIONS := -std=c++17 -Wall -pedantic

all: cxgoph-server cxgoph-client

cxgoph-server: cxgoph-server.o
	$(CXX) $(OPTIONS) cxgoph-server.o -o cxgoph-server

cxgoph-client: cxgoph-client.o
	$(CXX) $(OPTIONS) cxgoph-client.o -o cxgoph-client

cxgoph-server.o:
	$(CXX) $(OPTIONS) -c cxgoph-server.cpp

cxgoph-client.o:
	$(CXX) $(OPTIONS) -c cxgoph-client.cpp

clean:
	rm -rf *.o cxgoph-server cxgoph-client

