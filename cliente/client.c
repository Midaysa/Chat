#include <time.h>               // sleep
#include <fcntl.h>              // O_NONBLOCK, etc
#include <stdio.h>              // printf
#include <sys/queue.h>          // list macros
#include <stdlib.h>             // malloc, free
#include <stdbool.h>            // bool, true, false
#include <string.h>             // strlen
#include "../commons.h"         // error messages


#define BASIC_PERMISSIONS 0666
#define MSG_LEN 500


/* @Nombre: Quien
 * @Funcion: Esta orden muestra una lista de los usuarios conectados al
 * 			 servidor y el estado de cada uno.
			 La lista desplegada por la orden quien se mostrará en la ventana
			 de conversación.

 * @Entrada: Ninguna
 * @Salida:  Imprime en pantalla
 */

void who()

{
	// creamos la orden necesaria para enviarla al servidor servidor
	char* orderToSend = (char *) malloc(strlen(whoOrder));

	// Datos de prueba, estos seran substituidos por los que envie el pipe

	char* prueba_Lista = "Francisco - Estoy triste :( | Pepe - Estoy feliz =D";
	char* prueba_Length = "51";

	// Enviamos la orden al servidor de los datos que necesitamos

	char* buffer; //Variable en la que guardaremos los datos que vamos recibir del pipe
	int length;

	// Recibimos la longitud de los datos que vamos recibir, y
	// reservamos el espacio para recibir el archivo

	length = atoi(prueba_Length);
	buffer = (char *) malloc(length);

	// Recibimos La lista de usuarios

	strcpy(buffer, prueba_Lista);

	// Transformamos la lista

    // Prueba

    char** userName;
    char** userStatus;
    char** userAndStatus;
    int i = 0;

    userName = malloc(2 * sizeof(char *));
    userStatus = malloc(2 * sizeof(char *));
    userAndStatus = malloc(2 * sizeof(char *));

    // Prueba para ver como transformar la lista de usuarios que nos traeria el pipe

    // Hay dos formatos, los dos juntos o el estado y el nombre por su lado, es cuestion de escoger

    printf("Resultado De Funcion Who \n \n");

    for ( i = 0; i <= 1; i = i + 1 )
    {
    	userAndStatus[i] = malloc(strlen(getWord(buffer,"|",i)));
    	userAndStatus[i] = getWord(buffer,"|",i);

        userName[i] = malloc(strlen(getWord(userAndStatus[i],"-",0)));
        userName[i] = getWord(userAndStatus[i],"-",0);

        userStatus[i] = malloc(strlen(getWord(userAndStatus[i],"-",1)));
        userStatus[i] = getWord(userAndStatus[i],"-",1);

        printf("userAndStatus[%d] = %s\n", i,userAndStatus[i]);
        printf("userName[%d] = %s\n", i,userName[i]);
        printf("userStatus[%d] = %s\n", i,userStatus[i]);

    }

    // Liberamos la memoria utilizada para enviar la orden
    free(orderToSend);

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

void writeTo(char* username,char* userNameToWrite,char* message)

{
	// creamos la orden necesaria para enviarla al servidor servidor
	char* orderToSend = (char *) malloc(strlen(writeToOrder)
			+ strlen("-") + strlen(username)
			+ strlen(">") + strlen(userNameToWrite)
			+ strlen("|") + strlen(message));

	sprintf(orderToSend,"%s-%s>%s|%s",writeToOrder,username,userNameToWrite,message);

	// Liberamos la memoria utilizada para enviar la orden
	free(orderToSend);
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
	// creamos la orden necesaria para enviarla al servidor servidor
	char* orderToSend = (char *) malloc(strlen(whoOrder) + strlen("|") + strlen(client->nombre)
			+ strlen("|") + strlen(estado));
	sprintf(orderToSend,"%s|%s|%s",statusOrder,client->nombre,estado);

	// Enviar al servidor



	// Recibir mensaje de comfirmacion

	free(orderToSend);

}

/* @Nombre: Salir
 * @Funcion: Esta opción cierra el programa cliente y le notifica al servidor
 * que el usuario se desconectó. Se mostrará un mensaje indicando que el
 * usuario se desconectó en los clientes que estén conversando con él.
 * @Entrada: Cliente cliente que efectua la salida
 * @Salida:  Imprime en pantalla
 */

void logOut(char* userName)

{
	// Creamos la orden necesaria para enviarla al servidor servidor
	char* orderToSend = (char *) malloc(strlen(logOutOrder) + strlen("|") + strlen(userName));
	sprintf(orderToSend,"%s-%s",whoOrder,userName);

	// Enviar al servidor

	printf(LogOutMessage,"%s");

	// Liberamos la memoria utilizada para enviar la orden
	free(orderToSend);
	exit(0);
}



int main() {
    int len;
    char in_file_name[11], out_file_name[11]; // nombre de los pipes de entrada
                                              // y salida de cada usuario
    int fifo;                                 // pipe "servidor para
                                              // solicitar una nueva conexion
    int in_fd, out_fd;                        // pipes de entrada/salida
    int r;                                    // numero random entre
                                              // 1000000000 y 2000000000-1
    char message[MSG_LEN];
    
    srand(time(NULL));                        // inicializa semilla del random

    // Prueba Who

    who();

    // Prueba con lista de clientes

    int pruebaIterador;
    INIT_CLIENTLIST(listaPrueba);
    INIT_CLIENT(cliente1,"Pepe");
    INIT_CLIENT(cliente2,"Francisco");



    ClientList* clientListPointer = &listaPrueba;
    addNewClient(clientListPointer,"Pepe",0,0);
    addNewClient(clientListPointer, "Francisco",0,0);

    printf("Resultado De Funcion NewClient \n \n");

    for (pruebaIterador = 0; pruebaIterador < listaPrueba.size; pruebaIterador = pruebaIterador + 1 )
    {
    	printf("listaPrueba[%d] = %s - %s\n",
    			pruebaIterador,listaPrueba.client[pruebaIterador].nombre,
				listaPrueba.client[pruebaIterador].estado);
    }

    removeClient(clientListPointer, cliente1);

    printf("Resultado De Funcion removeClient \n \n");

    for (pruebaIterador = 0; pruebaIterador < listaPrueba.size; pruebaIterador = pruebaIterador + 1 )
    {
    	printf("listaPrueba[%d] = %s - %s\n",
    			pruebaIterador,listaPrueba.client[pruebaIterador].nombre,
				listaPrueba.client[pruebaIterador].estado);
    }

    // Prueba con lista de clientes

    // abrir pipe publico de conexiones nuevas del servidor
    fifo = open("servidor", O_WRONLY);
    
    if (fifo == -1) perror(getErrorMessage(openError,__LINE__,__FILE__));

    // convierte r a char y lo almacena en in_file_name
    r = rand() % 1000000000 + 1000000000;
    sprintf(in_file_name, "%d", r);

    r = rand() % 1000000000 + 1000000000;
    sprintf(out_file_name, "%d", r);
    printf("%s %s\n", in_file_name, out_file_name);

    // message = in_file_name + ' ' + out_file_name
    sprintf(message, "%s %s\n", in_file_name, out_file_name);

    // crear pipe (nominal) de entrada
    mkfifo(in_file_name, BASIC_PERMISSIONS, O_NONBLOCK);
    // crear pipe (nominal) de salida
    mkfifo(out_file_name, BASIC_PERMISSIONS | O_NONBLOCK);
    
    write(fifo, message, strlen(message));
    close(fifo);
    // abrir el pipe para leer datos
    in_fd = open(in_file_name, O_RDONLY);
    // abrir el pipe para enviar datos
    out_fd = open(out_file_name, O_WRONLY);


    char mensaje[50];
    len = read(in_fd, mensaje, 50);
    printf("len = %d\n", len);
    
    close(in_fd);
    close(out_fd);
    printf("mensaje = %s.\n", mensaje);




    // nos aseguramos de que el SO ya escribio el mensaje en el pipe antes de cerrar la app
    sleep(1);
    unlink(in_file_name);
    unlink(out_file_name);
}
