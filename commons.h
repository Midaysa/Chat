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

// CONSTANTES

extern const char *mkfifoError;
extern const char *selectError;
extern const char *openError;

extern const char *LogOutMessage;
extern const char *successMessage;

// Ordenes Cliente->Servidor

extern const char *whoOrder;
extern const char *writeToOrder;
extern const char *statusOrder;
extern const char *logOutOrder;

// ESTRUCTURAS

// --Client--

typedef struct Client

{
	char* nombre;
	char* estado;
	int friends[100];
    int in_fd, out_fd;

} Client;

// Defino un constructor para esta clase
#define INIT_CLIENT(new,name) Client new = {.nombre = name, .estado ="No Status", .friends = NULL, .in_fd = NULL, .out_fd =NULL}

// Tipo estructurado para la lista de usuarios

typedef struct ClientList

{
	struct Client* client;
	int size;

} ClientList;

// Defino un constructor para esta clase
#define INIT_CLIENTLIST(new) ClientList new = {.client = NULL, .size = 0}


// --Message--

typedef struct Message

{
	char* text;
	struct Client *sender;
	struct Client *reciever;

} Message;

// Defino un constructor para esta clase
#define INIT_MESSAGE(new,text,sender,reciever) Message new = {.text = text, .sender =sender, .reciever = reciever}

typedef struct MessageList

{
	struct Message* message;
	int size;

} MessageList;

// Defino un constructor para esta clase
#define INIT_MESSAGELIST(new) MessageList new = {.message = NULL, .size = 0}

// FUNCIONES

void addNewClient(ClientList* clientlist, Client client);
void removeClient(ClientList* clientlist, Client client);
char* getErrorMessage(const char* errorMessage,int line, char* file);
char* getWord(char* string,char* delimeter,int index);
int open_fifo(const char *fifo_name);


#endif /* COMMONS_H_ */
