.PHONY:all
.PHONY:clean

all: servidor cliente
servidor: servidor.o servidorFunc.o
	gcc -o servidor servidor.o servidorFunc.o

cliente: cliente.o clienteFunc.o
	gcc -o cliente cliente.o clienteFunc.o -lncurses

cliente.o: cliente.c clienteFunc.h
	gcc -c cliente.c 

clienteFunc.o: clienteFunc.c clienteFunc.h
	gcc -c clienteFunc.c

servidor.o: servidor.c servidorFunc.h
	gcc -c servidor.c

servidorFunc.o: servidorFunc.c servidorFunc.h
	gcc -c servidorFunc.c

clean:
	rm servidor cliente servidor.o servidorFunc.o cliente.o clienteFunc.o
