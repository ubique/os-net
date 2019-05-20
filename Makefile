GCC = g++
Client = client
Server = server

all:  $(Server) $(Client)

$(Server):
	$(GCC) $(Server).cpp -o $(Server)

$(Client):
	$(GCC) $(Client).cpp -o $(Client)

clean:
	rm $(Server) $(Client)
