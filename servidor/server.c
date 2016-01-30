#include <sys/select.h>         // select
#include <sys/stat.h>           // mkfifo
#include <time.h>               // sleep
#include <fcntl.h>              // O_NONBLOCK, etc
#include <stdio.h>              // printf
#include <sys/queue.h>          // list macros
#include <stdlib.h>             // malloc, free
#include <stdbool.h>            // bool, true, false
#include <unistd.h>             // unlink
#include <string.h>             // strlen
#include "../errorHandling.h"      // error messages
// #include "../cliente/client.h"      // error messages


#define MSG_LEN 500
#define BASIC_PERMISSIONS 0666
#define N 100
/*
// Tipo estructurado para la lista de usuarios

typedef struct ClientList

{
	Client* nombre;
	int size;

} ClientList;
*/
// crea y abre el pipe nominal fifo_name
// retorna el file descriptor del pipe creado
int open_fifo(const char *fifo_name) {
    int fifo;

    // eliminar el pipe nominal creado en alguna otra ejecución del server
    unlink(fifo_name);
    // esperar 1 seg para que el SO lo elimine completamente
    sleep(1);
    // crear pipe (nominal) de conexiones nuevas
    mkfifo(fifo_name, BASIC_PERMISSIONS | O_NONBLOCK);
    // abrir el pipe para leer conexiones entrantes
    fifo = open(fifo_name, O_RDONLY | O_NONBLOCK);

    //if (fifo == -1) perror(getError(mkfifoError,__LINE__,__FILE__));
    if (fifo == -1) perror("mkfifo");

    return fifo;
}


int main() {
    struct timeval tv;          // estructura para indicar el timeout de select
    int n;                      // 1 + file descriptor mas alto
    int rv;                     // return value del select, sirve para saber si
                                // retornó algo útil (>0),
                                // si hubo error (-1) o si hubo timeout (0)
    char message[MSG_LEN];      // string de datos enviados desde los clientes
                                // al servidor y viceversa
    int fifo;
    fd_set fdset;               // crear set de pipes nominales
    char in_file_name[11],      // nombre de los pipes de entrada
         out_file_name[11];

    printf("Iniciando servidor!\n");

    fifo = open_fifo("servidor");
    FD_ZERO(&fdset);            // limpiar el set de pipes nominales

    // crear contenedor de usuarios
    struct client {
        char username[30];
        char status[140];
        int friends[N];
        int in_fd, out_fd;
    };
    // EN CONSTRUCCION

    struct client c1;

    while (true) {
        printf("escuchando conexiones...\n");

        FD_SET(fifo, &fdset);           // agregar fifo al set de pipes
        n = 100;                        // sustituir por: fd mas alto +1

        // agregar los pipes de todos los usuarios al set de pipes
        // EN CONSTRUCCION

        tv.tv_sec = 1;                  // 1 seg de timeout con 0 microsegs
        tv.tv_usec = 0;

        rv = select(n, &fdset, NULL, NULL, &tv);

        if (rv == -1) {                 // error chequeando pipes
            perror((getErrorMessage(selectError,__LINE__,__FILE__)));
            perror("rvError");
        }
        else if (rv > 0) {              // existen archivos con datos para leer

            // si existe una nueva solicitud de conexion en el pipe 'fifo'
            if(FD_ISSET(fifo, &fdset)) {
                printf("fifo ready to read!");
                // agregar informacion del nuevo usuario al lista
                // leer del pipe fifo los id's de los pipes del cliente
                read(fifo, message, MSG_LEN);
                sscanf(message, "%s %s", out_file_name, in_file_name);
                printf("%s %s\n", in_file_name, out_file_name);
                close(fifo);
                fifo = open_fifo("servidor");

                char mensaje[] = "El servidor esta enviando datos...";
                printf("%s -- %d\n", mensaje, strlen(mensaje));
                c1.in_fd = open(in_file_name, O_RDONLY | O_NONBLOCK);
                c1.out_fd = open(out_file_name, O_WRONLY | O_NONBLOCK);
                write(c1.out_fd, mensaje, strlen(mensaje)+1);
            }
        }
    
        
        sleep(1);
    }
}


    //     revisar el pipe 'conexiones_nuevas'
    //     si existe una nueva solicitud de conexion:
    //         leer del pipe 'conexiones_nuevas' los id's de los pipes del cliente
    //         agregar informacion del nuevo usuario al diccionario
