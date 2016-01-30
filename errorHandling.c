/*
 * ErrorHandling.c
 *
 *  Created on: Jan 20, 2016
 *      Author: francisco
 */

#include <stdlib.h>

// Mensajes de error

const char *mkfifoError = "mkfifo Error";
const char *selectError = "select Error";
const char *openError = "open Error";

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
