/*
 * commons.h
 *
 *  Created on: Jan 30, 2016
 *      Author: francisco
 */

#ifndef COMMONS_H_
#define COMMONS_H_

extern const char *mkfifoError;
extern const char *selectError;
extern const char *openError;

const char *LogOutMessage;

char* getErrorMessage(const char* errorMessage,int line, char* file);
char* getWord(char* string,char* delimeter,int index);

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

#endif /* COMMONS_H_ */
