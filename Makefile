all: init_server init_client

init_server: server.c
	gcc -o server server.c

init_client: client.c
	gcc -o client client.c

run_server: init_server
	./server 127.0.0.1 1236 10

run_client: init_client
	./client 127.0.0.1 1236

clean:
	-rm -rfv server
	-rm -rfv client