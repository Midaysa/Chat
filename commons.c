/*
 * ErrorHandling.c
 *
 *  Created on: Jan 20, 2016
 *      Author: francisco
 */

#include <stdlib.h>
#include "commons.h"

// Mensajes de error

const char *mkfifoError = "mkfifo Error";
const char *selectError = "select Error";
const char *openError = "open Error";

const char *LogOutMessage = "Logging out... Thank You For Using Our Chat Services!";

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
