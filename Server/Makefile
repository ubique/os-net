all: Person.o Database.o Server.o server_main.o
	g++ Person.o Database.o Server.o server_main.o -o server

Person.o: ../Person/Person.cpp
	g++ -c ../Person/Person.cpp

Database.o: ../Database/Database.cpp
	g++ -c ../Database/Database.cpp

Server.o: Server.cpp
	g++ -c Server.cpp

server_main.o: server_main.cpp
	g++ -c server_main.cpp

clean:
	rm -rfv *.o server