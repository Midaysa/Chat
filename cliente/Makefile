client: client.o errorHandling.o
	gcc client.o errorHandling.o -o client

client.o:
	gcc -g -c client.c ../errorHandling.c
	
errorHandling.o:
	gcc -g -c ../errorHandling.c

clean:
	rm client.o errorHandling.o client
