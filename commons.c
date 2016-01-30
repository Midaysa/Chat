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
