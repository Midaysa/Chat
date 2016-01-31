/*
 * ErrorHandling.c
 *
 *  Created on: Jan 20, 2016
 *      Author: francisco
 */

#include <stdlib.h>
#include <string.h>             // strlen
#include <stdio.h>              // printf
#include "commons.h"
#include <time.h>               // sleep
#include <fcntl.h>              // O_NONBLOCK, etc
#include <string.h>             // strlen
#include <sys/stat.h>           // mkfifo


// Mensajes de error

const char *mkfifoError = "mkfifo Error";
const char *selectError = "select Error";
const char *openError = "open Error";

// Mensajes de sistema

const char *LogOutMessage = "Logging out... Thank You For Using Our Chat Services!";
const char *LogOutServerMessage = " has just logged out!";

// Ordenes Cliente->Servidor

const char *whoOrder = "whoOrder";
const char *writeToOrder = "writeToOrder";
const char *statusOrder = "statusOrder";
const char *logOutOrder = "logOutOrder";
const char *successMessage = "Operation Successfull";

char* getErrorMessage(const char* errorMessage,int line, char* file)

{
	// Convertimos el numero de linea en un String

	char* lineString;
	lineString = (char *) malloc(sizeof(line));
	sprintf(lineString, "%d", line);

	// Inicializamos la variable en la que devolveremos el error

	char* finalMessage;
	finalMessage = (char *) malloc(strlen(errorMessage) + strlen(" at line ")
			+ strlen(lineString) + strlen(" in file ") + strlen(file));

	// Iniciamos el mensaje con el error

	finalMessage = strcpy(finalMessage, errorMessage);

	// Agregamos la linea en la que ocurrio

	strcat(finalMessage, " at line ");
	strcat(finalMessage, lineString);

	// Agregamos el archivo donde ocurrio

	strcat(finalMessage, " in file ");
	strcat(finalMessage, file);

	// Retornamos el mensaje final

	return finalMessage;

}
// Separa un string con el delimitador seleccionado y devuelve la palabra seleccionada con index
char* getWord(char* string,char* delimeter,int index)

{
	// Si la lista es nula entonces abandonamos la funcion

	if (string == NULL)
	{
		return NULL;
	}

	char* stringCopy; //Copia del string original para no cambiarlo
	char* word; // Palabra a ser obtenida
	int i; // Iterador
	char** stringCopyPointer; // Apuntador a la copia del string necesaria para la funcion strsep
	char* stringCopyAddress;
	char* stringCopyPointerAddress;

	// Reservamos la memoria para la copia del string

	stringCopy = (char *) malloc(strlen(string));
	stringCopyAddress = stringCopy;
	strcpy(stringCopy, string);
	stringCopyPointer  = &stringCopy;
	word = (char *) malloc(strlen(string));

	// Recorremos la lista de palabras obteniendo sus palabras una a una

	for ( i = 0; i <= index; i = i + 1 )
	{
		strcpy(word, strsep(stringCopyPointer,delimeter));

		// Si la palabra es igual al string completo, nos salimos de la funcion

		if(stringCopy == NULL)
		{
			return word;
		}

	}

	// Si no ocurrieron ninguno de los casos anteriores entonces debimos obtener la palabra deseada

	free(stringCopyAddress);
	return word;
}

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


// Busca a un cliente en particular
Client* searchClient(ClientList* clientList, char* userName)
{
	int i;
	Client* clientToSearch = NULL;

	// Movemos los miembros del arreglo posteriores una casilla hacia atras
	for(i = 0; i < clientList->size; i = i + 1)
	{
		if (strcmp(clientList->client[i].nombre,userName) == 0)
		{
			clientToSearch = &(clientList->client[i]);
			return clientToSearch;
		}
	}
	return clientToSearch;
}

// Compara 2 clientes

int compareClients(Client client1, Client client2)
{
	Client* clientPointer;

	int resultName = !strcmp(client1.nombre,client2.nombre);
	if (resultName == 0) {resultName = 1;}

	int resultStatus = !strcmp(client1.estado,client2.estado);
	if (resultStatus == 0) {resultStatus = 1;}

	int resultInFd = client1.in_fd == client2.in_fd;

	int resultOutFd = client1.out_fd == client2.out_fd;

	int resultFriends = !memcmp(client1.friends, client2.friends, sizeof(client1.friends));
	if (resultFriends == 0) {resultFriends = 1;}

	return (resultName && resultStatus && resultInFd && resultOutFd && resultFriends);

}

// Agrega un cliente nuevo al arreglo de clientes

void addNewClient(ClientList* clientlist,char* userName,int in_fd,int out_fd)
{
	INIT_CLIENT(client,userName);

    // Reservamos la memoria para el nuevo estado
	client.estado = (char *) malloc(strlen("Sin Estado"));

	// Obtenemos el estado del cliente dado y lo actualizamos
	strcpy(client.estado, "Sin Estado");

	client.in_fd = in_fd;
	client.out_fd = out_fd;
	Client* clientPointer;

	if (clientlist->size > 0)
	{
		// Agregamos un slot nuevo en la memoria
		clientPointer = (Client *) realloc(clientlist->client,(sizeof(Client) * (clientlist->size + 1)));
		clientlist->client = clientPointer;
	}
	else
	{
		clientPointer = clientlist->client;
		// Agregamos un slot nuevo en la memoria
		clientlist->client = (Client *) malloc(sizeof(Client));
	}

	// Agregamos al nuevo usuario a la lista
	clientlist->client[clientlist->size] = client;
	clientlist->size = clientlist->size + 1;

}

// Remueve a un cliente del arreglo de clientes

void removeClient(ClientList* clientlist, Client client)
{
	int i;

	// Recorremos el arreglo

	for (i = 0; i < clientlist->size; i = i + 1)
	{
		// Cuando encontremos al cliente que buscamos procedemos a removerlo

		if (compareClients(clientlist->client[i],client))
		{

			int j;

			// Movemos los miembros del arreglo posteriores una casilla hacia atras
			for(j = i + 1; j < clientlist->size; j = j + 1)
			{
				clientlist->client[j - 1] = clientlist->client[j];
			}
			Client* arrayCopy;
			Client* pointerToArray = (clientlist->client);
			memmove(pointerToArray, pointerToArray, (clientlist->size)*sizeof(Client));


			Client* reallocPointerToArray;

			reallocPointerToArray = (Client *) realloc(pointerToArray,(sizeof(Client) * (clientlist->size)));

			// Disminuimos el tamaño
			clientlist->size = clientlist->size - 1;
			break;
		}
	}
}

// Agrega un cliente nuevo al arreglo de clientes

void addNewMessage(MessageList* messageList, Message message)
{
	Message* messagePointer;

	if (messageList->size > 0)
	{
		// Agregamos un slot nuevo en la memoria
		messagePointer = (Message *) realloc(messageList->message,(sizeof(Message) * (messageList->size + 1)));
		messageList->message = messagePointer;
	}
	else
	{
		messagePointer = messageList->message;
		// Agregamos un slot nuevo en la memoria
		messageList->message = (Message *) malloc(sizeof(Message));
	}

	// Agregamos al nuevo usuario a la lista
	messageList->message[messageList->size] = message;
	messageList->size = messageList->size + 1;

}
