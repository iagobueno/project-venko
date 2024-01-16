parametrosCompilacao=-Wall -std=c++20
parametrosBoost =
exec-server=exec-server

OBJ = mainServer.o ServerLib.o

all: $(exec-server)

$(exec-server): $(OBJ)
	g++ -o $(exec-server) $(OBJ) $(parametrosCompilacao) $(parametrosBoost)

mainServer.o: mainServer.cpp
	g++ -c mainServer.cpp $(parametrosCompilacao)

ServerLib.o: ServerLib.cpp ServerLib.hpp
	g++ -c ServerLib.cpp $(parametrosCompilacao)

run-server:
	./$(exec-server) 9896

clean:
	rm -f *.o *.gch $(exec-server)