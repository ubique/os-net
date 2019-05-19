server: Server.cpp
	c++ -std=c++14 $^ -o $@
	
client: Client.cpp
	c++ -std=c++14 $^ -o $@
	
clean:
	rm -rf client server 