server: server.cpp
	g++ $^ -o $@

client: client.cpp
	g++ $^ -o $@

clean:
	rm -rf client server