#### VARIABLES

CFLAGS=-Wall -std=c++20
EXEC=server client

SERVEROBJ = mainServer.o Server.o ServerLog.o
CLIENTOBJ = mainClient.o Client.o

#### ALL EXECUTABLES

all: $(EXEC)

#### CLIENT RULES

client: $(CLIENTOBJ)
	g++ -o client $(CLIENTOBJ) $(CFLAGS)
	
mainClient.o: mainClient.cpp
	g++ -c mainClient.cpp $(CFLAGS)

Client.o: Client.cpp Client.hpp
	g++ -c Client.cpp $(CFLAGS)

#### SERVER RULES

server: $(SERVEROBJ)
	g++ -o server $(SERVEROBJ) $(CFLAGS)
	
mainServer.o: mainServer.cpp
	g++ -c mainServer.cpp $(CFLAGS)

Server.o: Server.cpp Server.hpp
	g++ -c Server.cpp $(CFLAGS)

ServerLog.o: ServerLog.cpp ServerLog.hpp
	g++ -c ServerLog.cpp $(CFLAGS)

run-server:
	./server 9896

#### SANITIZER RULES

clean:
	rm -f *.o *.gch server