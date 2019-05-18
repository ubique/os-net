all: build runServer runClient

rebuild: clean build

build:
	mkdir build && cd build && cmake ../ && make

runServer:
	cd build && ./server localhost

runClient:
	cd build && ./client localhost

clean:
	rm -rf build