#include <sys/select.h>         // select
#include <sys/stat.h>           // mkfifo
#include <time.h>               // sleep
#include <fcntl.h>              // O_NONBLOCK, etc
#include <stdio.h>              // printf
#include <sys/queue.h>          // list macros
#include <stdlib.h>             // malloc, free
#include <stdbool.h>            // bool, true, false
#include <unistd.h>             // unlink
#include "errorHandling.h"      // error messages

#define MSG_LEN 500


int main()
{
    printf("Iniciando servidor!\n");

    // eliminar el pipe nominal creado en alguna otra ejecución del server
    unlink("servidor");
    // esperar 1 seg para que el SO lo elimine completamente
    sleep(1);
    // crear pipe (nominal) de conexiones nuevas
    mkfifo("servidor", 0666 | O_NONBLOCK);
    // abrir el pipe para leer conexiones entrantes
    int fifo = open("servidor", O_RDONLY | O_NONBLOCK);

    if (fifo == -1) perror(getError(mkfifoError,__LINE__,__FILE__));

    fd_set fdset;               // crear set de pipes nominales
    FD_ZERO(&fdset);            // limpiar el pool
    struct timeval tv;          // estructura para indicar el timeout de select
    int n;                      // 1 + file descriptor mas alto
    int rv;                     // return value del select, sirve para saber si
                                // retornó algo útil (>0),
                                // si hubo error (-1) o si hubo timeout (0)
    char string[MSG_LEN];       // string de datos enviados desde los clientes
                                // al servidor y viceversa

    // crear contenedor de usuarios
    // EN CONSTRUCCION


    while (true)
    {
        printf("escuchando conexiones...\n");
        sleep(1);

        FD_SET(fifo, &fdset);   // agregar fifo al set de pipes
        n = 100;

        // agregar los pipes de todos los usuarios al set de pipes

        tv.tv_sec = 1;        // 1 seg de timeout con 0 microsegs
        tv.tv_usec = 0;

        rv = select(n, &fdset, NULL, NULL, &tv);

        if (rv == -1)
        {
        	// error chequeando pipes
            perror(getError(selectError,__LINE__,__FILE__));
        }
        else if (rv > 0)
        {
        	// existen archivos con datos para leer
            if(FD_ISSET(fifo, &fdset))
            {
                printf("fifo ready to read!");
                // read code

            }
        }
    //     revisar el pipe 'conexiones_nuevas'
    //     si existe una nueva solicitud de conexion:
    //         leer del pipe 'conexiones_nuevas' los id's de los pipes del cliente
    //         agregar informacion del nuevo usuario al diccionario
    }
}
