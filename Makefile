#Makefile
Compilator := g++
Flags := -std=c++11
Client := client
Server := server

all: $(Server) $(Client)

$(Server)Run: $(Server)
	./$(Server)

$(Client)Run: $(Client)
	./$(Client) message

$(Server): $(Server).cpp
	$(Compilator) $(Flags) $(Server).cpp -o $(Server)

$(Client): $(Client).cpp
	$(Compilator) $(Flags) $(Client).cpp -o $(Client)

clean:
	rm $(Server) $(Client)
