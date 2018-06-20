all: servidor cliente clean

file:
	rm -rf bin
	mkdir bin

cliente: file cliente.o shared.o
	gcc cliente.o shared.o -o ./bin/cliente -lrt -pthread

servidor: file servidor.o shared.o
	gcc shared.o servidor.o -o ./bin/servidor -lrt -pthread

.PHONY: clean
clean:
	rm -f *.o