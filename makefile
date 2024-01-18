PARAMETROSCOMPILACAO=-Wall -std=c++20
EXEC=server client

SERVEROBJ = MAINSERVER.o Server.o ServerLog.o
CLIENTOBJ = mainClient.o

all: $(EXEC)

client: $(CLIENTOBJ)
	g++ -o client $(CLIENTOBJ) $(PARAMETROSCOMPILACAO)
	
mainClient.o: mainClient.cpp
	g++ -c mainClient.cpp $(PARAMETROSCOMPILACAO)

server: $(SERVEROBJ)
	g++ -o server $(SERVEROBJ) $(PARAMETROSCOMPILACAO)
	
MAINSERVER.o: MAINSERVER.cpp
	g++ -c MAINSERVER.cpp $(PARAMETROSCOMPILACAO)

Server.o: Server.cpp Server.hpp
	g++ -c Server.cpp $(PARAMETROSCOMPILACAO)

ServerLog.o: ServerLog.cpp ServerLog.hpp
	g++ -c ServerLog.cpp $(PARAMETROSCOMPILACAO)

run-server:
	./server 9896

clean:
	rm -f *.o *.gch server