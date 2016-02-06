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
extern const char *argNumError;
extern const char *argOrdError;

extern const char *LogOutMessage;
extern const char *LogOutServerMessage;
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
	int friends[N];
    int in_fd, out_fd;

} Client;

// Defino un constructor para esta clase
#define INIT_CLIENT(new,name) Client new = {.nombre = name, .estado =NULL, .in_fd = 0, .out_fd =0}

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
#define INIT_MESSAGE(new,text,sender,reciever) Message new = {text,sender, reciever}

typedef struct MessageList

{
	struct Message* message;
	int size;

} MessageList;

// Defino un constructor para esta clase
#define INIT_MESSAGELIST(new) MessageList new = {.message = NULL, .size = 0}

// FUNCIONES

char* getErrorMessage(const char* errorMessage,int line, char* file);
char* getWord(char* string,char* delimeter,int index);
int open_fifo(const char *fifo_name);
// Recibe una entrada de un pipe optimizando la memoria utilizada
void recieveFromPipe(char *bufferToRecieve, char* in_file_name);
void sendThroughPipe(char *bufferToSend, char* out_file_name);

// -- Client --

Client* searchClient(ClientList* clientList,char* userName);
int compareClients(Client client1, Client client2);
void addNewClient(ClientList* clientlist,char* userName,int in_fd,int out_fd);
void removeClient(ClientList* clientlist, Client client);

// -- Message --

void addNewMessage(MessageList* messageList, Message message);



#endif /* COMMONS_H_ */
