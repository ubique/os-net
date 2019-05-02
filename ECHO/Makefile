DEFAULT_IP=127.0.0.1
#DEFAULT_PORT=7 # need root privileges for low ports
DEFAULT_PORT=6666


.PHONY: all clean

all: server client

clean:
	-rm -rfv server
	-rm -rfv client

server: echo_server.c
	cc -o $@ $<

client: echo_client.c
	cc -o $@ $<

.PHONY: test_run_server test_run_client

test_run_server: server
	./server ${DEFAULT_IP} ${DEFAULT_PORT}

test_run_client: client
	./client ${DEFAULT_IP} ${DEFAULT_PORT} "TEXT"
	./client ${DEFAULT_IP} ${DEFAULT_PORT} "The quick brown fox jumps over the lazy dog"
	./client ${DEFAULT_IP} ${DEFAULT_PORT} "Sphinx of black quartz, judge my vow"
	./client ${DEFAULT_IP} ${DEFAULT_PORT} "The five boxing wizards jump quickly"
	./client ${DEFAULT_IP} ${DEFAULT_PORT} "Cwm fjord bank glyphs vext quiz"
	./client ${DEFAULT_IP} ${DEFAULT_PORT} "Mr Jock, TV quiz PhD, bags few lynx"

