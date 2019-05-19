CXX=g++
NAME=protocol
OUTPUT=output

.PHONY: all clean

all:
	@rm -rf ${OUTPUT}
	@mkdir ${OUTPUT}
	@${CXX} main.cpp tcp/*.cpp server/*.cpp client/*.cpp protocol/*.cpp utils/*.cpp -o ${OUTPUT}/${NAME}

clean:
	@rm -rf ${OUTPUT}
