#include <sys/select.h>         // select
#include <sys/stat.h>           // mkfifo
#include <time.h>               // sleep
#include <fcntl.h>              // O_NONBLOCK, etc
#include <stdio.h>              // printf
#include <stdlib.h>             // malloc, free
#include <stdbool.h>            // bool, true, false
#include <unistd.h>             // unlink
#include <string.h>             // strlen
#include "../commons.h"      // error messages

#define MSG_LEN 500
#define BASIC_PERMISSIONS 0666
#define N 100


/* @Nombre: Quien
 * @Funcion: Esta orden muestra una lista de los usuarios conectados al
 * 			 servidor y el estado de cada uno.
			 La lista desplegada por la orden quien se mostrará en la ventana
			 de conversación.

 * @Entrada: ClientList* clientlist: lista de clientes del sistema
 * @Entrada: int pipeId: pipe que se utilizara para enviar la informacion
 * @Salida:  Imprime en pantalla
 */

void whoServer(ClientList* clientlist,int pipeId)

{
	// FORMATO DE arguments == NULL

	int i;
	char* buffer;
	int bufferSize;

	// Calculamos el tamaño del buffer
	for (i = 0; i < clientlist ->size; i = i + 1)
	{
		bufferSize = bufferSize + strlen(clientlist->client[i].nombre);

		bufferSize = bufferSize + strlen("-");

		bufferSize = bufferSize + strlen(clientlist->client[i].estado);

		// Si no estamos en el ultimo usuario, agregamos un separador
		if (i < clientlist ->size -1)
		{
			bufferSize = bufferSize + strlen("|");
		}
	}

	printf("bufferSize = %d \n",bufferSize);

	// Reservamos la memoria para el buffer

	buffer = (char *) malloc(bufferSize);

	// Vamos creando el mensaje
	for (i = 0; i < clientlist ->size; i = i + 1)
	{
		if (i == 0)
		{
			sprintf(buffer,"%s-%s",clientlist->client[i].nombre,clientlist->client[i].estado);
		}
		else
		{
			sprintf(buffer,"%s%s-%s",buffer,clientlist->client[i].nombre,clientlist->client[i].estado);
		}

		// Si no estamos en el ultimo usuario, agregamos un separador
		if (i < clientlist ->size -1)
		{
			sprintf(buffer,"%s|",buffer);
		}
	}


	printf("buffer = %s \n \n",buffer);

}

/* @Nombre: Escribir
 * @Funcion: Esta orden toma el nombre de un usuario conectado como argumento
 * 			 e indica que se quiere conversar con él. Los mensajes enviados
 * 			 después de ejecutar la orden serán dirigidos a ese usuario.
 *
 * 			 Si se vuelve a ejecutar la orden escribir con otro nombre de
 * 			 usuario, los mensajes ahora serán dirigidos al nuevo usuario.
 *
 * 			 De este modo, será posible mantener varias conversaciones con
 * 			 distintos usuarios en la misma pantalla.
 *
 * @Entrada: Cliente cliente: cliente que escribe el mensaje
 * @Entrada: Cliente clienteAEscribir: cliente al que se le escribe el mensaje
 * @Salida:  Imprime en pantalla
 */

void writeToServer(ClientList* clientList,MessageList* messageList,char* arguments)

{

	// FORMATO DE arguments == 'Francisco|Pepe|Hola ¿como estas?'

	char* message;
	Client* client;
	Client* clientToWrite;

	// Obtenemos al cliente que envia el mensaje
	client = searchClient(clientList, getWord(arguments,"|",0));

	// Obtenemos al cliente que recibe el mensaje
	clientToWrite = searchClient(clientList, getWord(arguments,"|",1));

	// Obtenemos el mensaje a enviar
	message = getWord(arguments,"|",2);


	// Creamos el mensaje
	INIT_MESSAGE(newMessage,message,client,clientToWrite);
	// Agregamos el mensaje a la lista
	addNewMessage(messageList, newMessage);

}

/* @Nombre: Estoy
 * @Funcion: Esta orden cambia el estado del usuario.
 * Si un cliente está conversando con determinado usuario
 * (es decir, si la última orden escribir ejecutada en el cliente fue a
 * ese usuario), y este usuario cambia de estado, se debe mostrar
 * una notificación en el cliente con el nuevo estado del usuario.

 * @Entrada: Cliente cliente: cliente que actualiza su estado
 * @Entrada: char* estado: el estado nuevo del cliente
 * @Salida:  Imprime en pantalla
 */

void statusServer(ClientList* clientList,char* arguments)

{

	// FORMATO DE arguments == 'Francisco|¡Estoy aburrido!'

	Client* client;
	char* status;

	// Obtenemos al cliente que envia el mensaje
	client = searchClient(clientList, getWord(arguments,"|",0));

	// Obtenemos el mensaje a enviar
	status = getWord(arguments,"|",1);

	// Si ya existe un estado anterior libero la memoria

	if (client -> estado != NULL)
	{
		// Liberamos la memoria utilizada por el
		free(client -> estado);
	}
	// Reservamos la memoria para el nuevo estado
	client -> estado = (char *) malloc(strlen(status));

	// Obtenemos el estado del cliente dado y lo actualizamos
	strcpy(client -> estado, status);
	free(status);

}

/* @Nombre: Salir
 * @Funcion: Esta opción cierra el programa cliente y le notifica al servidor
 * que el usuario se desconectó. Se mostrará un mensaje indicando que el
 * usuario se desconectó en los clientes que estén conversando con él.
 * @Entrada: Cliente cliente que efectua la salida
 * @Salida:  Imprime en pantalla
 */

void logOutServer(ClientList* clientList,char* arguments)

{
	// FORMATO DE arguments == 'Francisco'

	Client* client;
	char* status;

	// Obtenemos al cliente que envia el mensaje
	client = searchClient(clientList, arguments);

	removeClient(clientList, *client);
	printf("User %s %s\n", arguments , LogOutServerMessage );
}



int main(int argc, char **argv)
{
    struct timeval tv;          // estructura para indicar el timeout de select
    int n;                      // 1 + file descriptor mas alto
    int rv;                     // return value del select, sirve para saber si
                                // retornó algo útil (>0),
                                // si hubo error (-1) o si hubo timeout (0)
    char message[MSG_LEN];      // string de datos enviados desde los clientes
                                // al servidor y viceversa
    int fifo;
    fd_set fdset;               // crear set de pipes nominales
    char* in_file_name;     	// nombre de los pipes de entrada
    char* out_file_name;		// nombre de los pipes de entrada



	if (argc == 2)
	{
		in_file_name = (char *) malloc(strlen(argv[1]));
		strcpy(in_file_name,argv[1]);

	}

	else if (argc == 1)
	{
		in_file_name = (char *) malloc(strlen("/tmp/servidor"));
		strcpy(in_file_name,"/tmp/servidor");
	}

	else
	{
		printf("%s", argNumError);
	}

    printf("Iniciando servidor!\n");

    fifo = open_fifo(in_file_name);
    FD_ZERO(&fdset);                     // limpiar el set de pipes nominales

    Client c1;

    // EN CONSTRUCCION

    while (true) {
        printf("escuchando conexiones...\n");

        FD_SET(fifo, &fdset);           // agregar fifo al set de pipes
        n = 100;                        // sustituir por: fd mas alto +1

        // agregar los pipes de todos los usuarios al set de pipes
        // EN CONSTRUCCION

        tv.tv_sec = 1;                  // 1 seg de timeout con 0 microsegs
        tv.tv_usec = 0;

        rv = select(n, &fdset, NULL, NULL, &tv);

        if (rv == -1)
        {   // error chequeando pipes
            perror((getErrorMessage(selectError,__LINE__,__FILE__)));
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
                fifo = open_fifo(in_file_name);

                char mensaje[] = "El servidor esta enviando datos...";
                printf("%s -- %zd\n", mensaje, strlen(mensaje));
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
