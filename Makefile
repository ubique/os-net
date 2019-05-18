server: server.cpp
	c++ -std=c++14 $^ -o $@

client: client.cpp
	c++ -std=c++14 $^ -o $@

clean:
	rm -rf client server