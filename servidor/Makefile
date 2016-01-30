server: server.o errorHandling.o
	gcc server.o errorHandling.o -o server

server.o:
	gcc -g -c server.c ../errorHandling.c ../cliente/client.h

errorHandling.o:
	gcc -g -c ../errorHandling.c

clean:
	rm server.o server
