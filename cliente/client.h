/*
 * client.h
 *
 *  Created on: Jan 29, 2016
 *      Author: francisco
 */

#ifndef CLIENT_H_
#define CLIENT_H_

const char *LogOutMessage = "Logging out... Thank you for using our services!\n";

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


#endif /* CLIENT_H_ */
