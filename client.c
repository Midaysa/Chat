#include <time.h>               // sleep
#include <fcntl.h>              // O_NONBLOCK, etc
#include <stdio.h>              // printf
#include <sys/queue.h>          // list macros
#include <stdlib.h>             // malloc, free
#include <stdbool.h>            // bool, true, false
#include "errorHandling.h"      // error messages

typedef struct Client

{
	char* nombre;
	char* estado;
	struct Client *siguiente;
	struct Client *anterior;

} Client;

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

void writeTo(Client client,Client clientToWrite)

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

void status(Client client, char* estado)

{

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

}

int main() {
    int fifo = open("servidor", O_WRONLY);
    if (fifo == -1) perror(getError(openError,__LINE__,__FILE__));
    
    printf("here\n");
    write(fifo, "hola!", 6);
    printf("now here\n");
    sleep(5);
}
