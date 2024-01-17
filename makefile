PARAMETROSCOMPILACAO=-Wall -std=c++20
EXEC=server client

SERVEROBJ = mainServer.o ServerLib.o
CLIENTOBJ = mainClient.o

all: $(EXEC)

client: $(CLIENTOBJ)
	g++ -o client $(CLIENTOBJ) $(PARAMETROSCOMPILACAO)
	
mainClient.o: mainClient.cpp
	g++ -c mainClient.cpp $(PARAMETROSCOMPILACAO)

server: $(SERVEROBJ)
	g++ -o server $(SERVEROBJ) $(PARAMETROSCOMPILACAO)
	
mainServer.o: mainServer.cpp
	g++ -c mainServer.cpp $(PARAMETROSCOMPILACAO)

ServerLib.o: ServerLib.cpp ServerLib.hpp
	g++ -c ServerLib.cpp $(PARAMETROSCOMPILACAO)

run-server:
	./server 9896

clean:
	rm -f *.o *.gch server