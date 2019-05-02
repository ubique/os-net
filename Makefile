all: server client
	

server: src/main.cpp src/sftp_server.cpp src/sftp_server.h src/database.cpp src/database.h
	g++ -fsanitize=address,undefined $^ -o $@

client: src/client.cpp
	g++ -fsanitize=address,undefined $^ -o $@

	
clean:
	rm -rf server client
