chat: chat.o
	gcc chat.o -o chat -lncurses

chat.o: chat.c chat.h
	gcc -c -g chat.c

clean:
	rm chat.o chat
