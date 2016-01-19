#include <time.h>               // sleep
#include <fcntl.h>              // O_NONBLOCK, etc
#include <stdio.h>              // printf
#include <sys/queue.h>          // list macros
#include <stdlib.h>             // malloc, free
#include <stdbool.h>            // bool, true, false


int main() {
    int fifo = open("servidor", O_WRONLY);
    if (fifo == -1) perror("open");
    
    printf("here\n");
    write(fifo, "hola!", 6);
    printf("now here\n");
    sleep(5);
}