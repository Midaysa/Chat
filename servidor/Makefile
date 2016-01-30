server: server.o errorHandling.o
	gcc server.o errorHandling.o -o server

server.o:
	gcc -c server.c ../errorHandling.c

errorHandling.o:
	gcc -c ../errorHandling.c

clean:
	rm server.o server
