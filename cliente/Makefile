client: client.o commons.o
	gcc client.o commons.o -o ../executables/client -lncurses

client.o:
	gcc -g -c client.c ../commons.c ../commons.h 
	
commons.o:
	gcc -g -c ../commons.c ../commons.h

clean:
	rm client.o commons.o client
