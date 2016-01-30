/*
 * commons.h
 *
 *  Created on: Jan 30, 2016
 *      Author: francisco
 */

#ifndef COMMONS_H_
#define COMMONS_H_

#define MSG_LEN 500
#define BASIC_PERMISSIONS 0666
#define N 100

extern const char *mkfifoError;
extern const char *selectError;
extern const char *openError;

const char *LogOutMessage;
const char *successMessage;

// Ordenes Cliente->Servidor

const char *whoOrder;
const char *writeToOrder;
const char *statusOrder;
const char *logOutOrder;

char* getErrorMessage(const char* errorMessage,int line, char* file);
char* getWord(char* string,char* delimeter,int index);
int open_fifo(const char *fifo_name);

typedef struct Client

{
	char* nombre;
	char* estado;
	int friends[100];
    int in_fd, out_fd;

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
