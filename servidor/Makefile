server: server.o commons.o
	gcc server.o commons.o -o ../executables/server

server.o:
	gcc -g -c server.c ../commons.c ../commons.h

commons.o:
	gcc -g -c ../commons.c ../commons.h

clean:
	rm server.o ../executables/server commons.o
