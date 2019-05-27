CXXFLAGS+=-std=c++14 -Wall -O2
CXX=g++
BUILD=build

.PHONY: all clean

all: make_dir server client 
	
server: make_dir fd.h server.h server.cpp	
	$(CXX) fd.h server.h server.cpp -o $(BUILD)/server

client: make_dir fd.h client.h client.cpp
	$(CXX) fd.h client.h client.cpp -o $(BUILD)/client

make_dir:
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)