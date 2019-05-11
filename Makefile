all: build run

rebuild: clean build

build:
	mkdir build && cd build && cmake ../ && make

run:
	

clean:
	rm -rf build