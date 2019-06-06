all: server client
	

server: src/main.cpp src/sftp_server.cpp src/sftp_server.h src/database.cpp src/database.h src/rwutil.h src/rwutil.cpp
	g++ -fsanitize=address,undefined $^ -o $@

client: src/client.cpp src/rwutil.cpp src/rwutil.h
	g++ -fsanitize=address,undefined $^ -o $@

	
clean:
	rm -rf server client
