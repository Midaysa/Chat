server: server.o commons.o
	gcc server.o commons.o -o server

server.o:
	gcc -g -c server.c ../commons.c ../commons.h

commons.o:
	gcc -g -c ../commons.c ../commons.h

clean:
	rm server.o server commons.o
