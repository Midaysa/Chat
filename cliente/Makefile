client: client.o errorHandling.o
	gcc client.o errorHandling.o -o client

client.o:
	gcc -c client.c ../errorHandling.c
	
errorHandling.o:
	gcc -c ../errorHandling.c

clean:
	rm client.o client