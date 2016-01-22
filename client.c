#include <time.h>               // sleep
#include <fcntl.h>              // O_NONBLOCK, etc
#include <stdio.h>              // printf
#include <sys/queue.h>          // list macros
#include <stdlib.h>             // malloc, free
#include <stdbool.h>            // bool, true, false
#include "errorHandling.h"      // error messages


#define BASIC_PERMISSIONS 0666
#define MSG_LEN 500


const char *LogOutMessage = "Logging out... Thank you for using our services!\n";

typedef struct Client

{
	char* nombre;
	char* estado;
	struct Client *siguiente;
	struct Client *anterior;

} Client;

// Defino un constructor para esta clase
#define INIT_CLIENT(new) Client new = {.nombre = NULL, .estado =NULL, .siguiente = NULL, .anterior = NULL}

typedef struct Message

{
	char* text;
	struct Client *sender;
	struct Client *reciever;

} Message;

// Defino un constructor para esta clase
#define INIT_MESSAGE(new) Message new = {.text = NULL, .sender =NULL, .reciever = NULL}


/* @Nombre: Quien
 * @Funcion: Esta orden muestra una lista de los usuarios conectados al
 * 			 servidor y el estado de cada uno.
			 La lista desplegada por la orden quien se mostrará en la ventana
			 de conversación.

 * @Entrada: Cliente cliente: cliente que hace la busqueda
 * @Salida:  Imprime en pantalla
 */

void who(Client client)

{

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

void writeTo(Client* client,Client* clientToWrite)

{

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

void status(Client* client, char* estado)

{
	// Si ya existe un estado anterior libero la memoria

	if (client -> estado)
	{
		// Liberamos la memoria utilizada por el
		free(client -> estado);
	}
	// Reservamos la memoria para el nuevo estado
	client -> estado = (char *) malloc(strlen(estado));
	// Obtenemos el estado del cliente dado y lo actualizamos
	strcpy(client -> estado, estado);

}

/* @Nombre: Salir
 * @Funcion: Esta opción cierra el programa cliente y le notifica al servidor
 * que el usuario se desconectó. Se mostrará un mensaje indicando que el
 * usuario se desconectó en los clientes que estén conversando con él.
 * @Entrada: Cliente cliente que efectua la salida
 * @Salida:  Imprime en pantalla
 */

void logOut(Client client)

{
	printf(LogOutMessage,"%s");
	exit(0);
}



int main() {
    char in_file_name[11], out_file_name[11]; // nombre de los pipes de entrada
                                              // y salida de cada usuario
    int fifo = open("servidor", O_WRONLY);    // abre el pipe "servidor para
                                              // solicitar una nueva conexion
    int r;                                    // numero random entre
                                              // 1000000000 y 2000000000-1
    int fifo_in, fifo_out;                    // pipes de entrada/salida
    char message[MSG_LEN];


    srand(time(NULL));                        // inicializa semilla del random

    // convierte r a char y lo almacena en in_file_name
    r = rand() % 1000000000 + 1000000000;
    sprintf(in_file_name, "%d", r);

    r = rand() % 1000000000 + 1000000000;
    sprintf(out_file_name, "%d", r);
    printf("%s %s\n", in_file_name, out_file_name);

    // message = in_file_name + ' ' + out_file_name
    sprintf(message, "%s %s\n", in_file_name, out_file_name);

    // crear pipe (nominal) de entrada
    mkfifo(in_file_name, BASIC_PERMISSIONS | O_NONBLOCK);
    // crear pipe (nominal) de salida
    mkfifo(out_file_name, BASIC_PERMISSIONS | O_NONBLOCK);
    // abrir el pipe para leer las conexiones entrantes
    fifo_in = open(in_file_name, BASIC_PERMISSIONS | O_NONBLOCK);
    // abrir el pipe para enviar datos
    fifo_out = open(out_file_name, BASIC_PERMISSIONS | O_NONBLOCK);


    if (fifo == -1) perror (getError(openError,__LINE__,__FILE__));

    write(fifo, message, strlen(message));

    // nos aseguramos de que el SO ya escribio el mensaje en el pipe antes de cerrar la app
    sleep(1);
}
