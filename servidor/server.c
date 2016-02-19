/*
 * server.c
 *
 *	Programa del servidor que se encargara de recibir datos de los clientes que se conecten a el y distribuirlos de
 *	manera automatizada
 *
 *  Created on: Jan 18, 2016
 *      Author: francisco y midaysa
 */

#include <sys/select.h>         // select
#include <sys/stat.h>           // mkfifo
#include <time.h>               // sleep
#include <fcntl.h>              // O_NONBLOCK, etc
#include <stdio.h>              // printf
#include <stdlib.h>             // malloc, free
#include <stdbool.h>            // bool, true, false
#include <unistd.h>             // unlink
#include <string.h>             // strlen
#include <signal.h>				// Signal
#include "../commons.h"         // Mensajes De Error y funciones comunes

#define MSG_LEN 500
#define STATUS_LEN 100
#define BASIC_PERMISSIONS 0666
#define N 20

/* Estructura de datos que contiene todos los datos de un usuario que se haya registrado en el servidor */
struct client {
    char username[30];
	char status[140];
    char actualConversation[30];
    int in_fd, out_fd;
} clients[N];

void initialize();
void sendMessage(char username[], char message[]);
void writeFull(char *token, char dst[]);
const char* searchUser(char username[]);
void login(char username[], int in_fd, int out_fd);
void logout(char username[]);
static void sigkillHandler(int signo);

int fifo;                   			// fd del pipe del servidor
char server_pipe_name[100]; 			// nombre del pipe nominal del servidor

int main(int argc, char *argv[]) {
    struct timeval tv;          		// estructura para indicar el timeout de select
    int n;                      		// 1 + file descriptor mas alto
    int rv;                     		// return value del select, sirve para saber si
                                		// retornó algo útil (>0),
                                		// si hubo error (-1) o si hubo timeout (0)
    char message[MSG_LEN];      		// string de datos enviados desde los clientes
                                		// al servidor y viceversa
    char tmp[MSG_LEN];          		// string temporal para agregar origen de mensaje
    fd_set fdset, error_fdset;  		// crear sets de pipes nominales
    char in_file_name[11],     			// nombre de los pipes de entrada
         out_file_name[11];
    int in_fd, out_fd;          		// file descriptors temporales para clientes
    char username[30];          		// nombre temporal para clientes
    int i, j;                   		// variables de iteración simple
    char *token;						// Variable usada para guardar las palabras separadas del
    									// input del usuario
    signal(SIGPIPE, SIG_IGN);			// Manejador de senales para SIGPIPE
	signal(SIGINT, sigkillHandler);		// Manejador de senales para SIGINT
	signal(SIGABRT, sigkillHandler);	// Manejador de senales para SIGINT
	signal(SIGTERM, sigkillHandler);	// Manejador de senales para SIGINT

    /* Si se dio un nombre de server lo guardamos */
    if (argc == 2)
    {
        strcpy(server_pipe_name, argv[1]);
    }
    /* Sino usamos el nombre por defecto */
    else
    {
        strcpy(server_pipe_name, defaultServer);
    }

    printf("%s\n",serverStartMessage);

    fifo = openFifo(server_pipe_name);
    if (fifo == -1) perror(getErrorMessage(openError,__LINE__,__FILE__));
    tv.tv_sec = 1;                  	// 1 seg de timeout con 0 microsegs
    tv.tv_usec = 0;
    initialize();

    /* Ciclo Principal */
    while (true)
    {
        FD_ZERO(&fdset);                // limpiar el set de pipes nominales
        FD_ZERO(&error_fdset);
        FD_SET(fifo, &fdset);           // agregar fifo al set de pipes
        n = 100;                        // sustituir por: fd mas alto +1

        strcpy(message, "");

        for (i=0; i<N; i++)
        {
        	/* agregar pipes de todos los usuarios al fd_set */
            if (strlen(clients[i].username) > 0)
            {
                FD_SET(clients[i].in_fd, &fdset);
            }
        }

        rv = select(n, &fdset, NULL, &error_fdset, &tv);

        if (rv == -1)
        {
            perror(getErrorMessage(rvError,__LINE__, __FILE__));
            for (i=0; i<N; i++)
            {
                if (FD_ISSET(clients[i].in_fd, &error_fdset))
                {
                    printf("Error on: %s| %d | %d\n", clients[i].username, 
                                                      clients[i].in_fd,
                                                      clients[i].out_fd);
                }
            }
        }
        else if (rv > 0)
        {
        	/* existen archivos con datos para leer
               si existe una nueva solicitud de conexion en el pipe 'fifo' */
            if(FD_ISSET(fifo, &fdset))
            {
                /* agregar informacion del nuevo usuario al lista
                   leer del pipe fifo los id's de los pipes del cliente */
                read(fifo, message, MSG_LEN);
                sscanf(message, "%s %s %s\n", username, out_file_name, in_file_name);
                close(fifo);

                /* Si encontramos a un usuario con el mismo nombre rechazamos la conexion */
                if (strcmp(searchUser(username),successMessage) == 0)
                {
                	write(out_fd, userNameNotAvaible, strlen(userNameNotAvaible));
                }
                else
                {
                    fifo = openFifo(server_pipe_name);
                    in_fd = open(in_file_name, O_RDONLY | O_NONBLOCK);
                    out_fd = open(out_file_name, O_WRONLY | O_NONBLOCK);
                    login(username, in_fd, out_fd);
                }
            }
            /* Recorremos el set de pipes */
            for (i=0; i<N; i++)
            {
            	/* Se recibio input de parte del usuario */
                if (FD_ISSET(clients[i].in_fd, &fdset))
                {
                    strcpy(message, "");
                    read(clients[i].in_fd, message, MSG_LEN);
                    token = strtok(message, " ");      // token = primera palabra del comando

                    /* Detectamos si el usuario no esta en linea */
                    if (token == NULL && strcmp(clients[i].username,"") != 0)
                    {
                        logout(clients[i].username);
                        continue;
                    }

                    /* Cambio de estado */
                    if (strcmp(token, ordenEstoy) == 0)
                    {
						token = strtok(NULL, " ");
						writeFull(token, message);
						strcpy(clients[i].status, message);

						sprintf(message, "%s ha cambiado de estado a: %s", clients[i].username, clients[i].status);

						/* Buscamos a los usuarios que estan hablando con el usuario que cambio su estado */
					    for (j=0; j<N; j++)
					    {
					        if (strcmp(clients[j].actualConversation, clients[i].username) == 0)
					        {
					        	write(clients[j].out_fd, message, MSG_LEN);
					        }
					    }

                    }
                    /* Mostrar Lista de usuarios */
                    else if (strcmp(token, ordenQuien) == 0)
                    {
                        strcpy(message, "");

                        strcat(message, "\nUsuarios Conectados \n\n");

                        for (j=0; j<N; j++)
                        {
                            if (strlen(clients[j].username) > 0)
                            {
								strcat(message, clients[j].username);
								strcat(message, ":");
								strcat(message, clients[j].status);
								strcat(message, "\n");
                            }
                        }
                        write(clients[i].out_fd, message, MSG_LEN);
                    }
                    /* Cambio de conversacion */
                    else if (strcmp(token, ordenCambiarConversacion) == 0)
                    {
                        token = strtok(NULL, " ");
                        strcpy(username, token);
                        if (strcmp(searchUser(username),successMessage) == 0)
                        {
                        	strcpy(clients[i].actualConversation,username);
                        	write(clients[i].out_fd, successMessage, MSG_LEN);
                        }
                        else
                        {
                        	write(clients[i].out_fd, userNotFoundMessage, MSG_LEN);
                        }
                    }
                    /* Envio de un mensaje */
                    else if (strcmp(token, ordenEscribir) == 0)
                    {
                    	/* Obtenemos el nombre de usuario */
                        token = strtok(NULL, " ");
                        strcpy(username, token);
                        token = strtok(NULL, " ");
                        writeFull(token, message);
                        /* Caso 1: Se encuentra al usuario en el servidor y se le envia el mensaje */
                        if (strcmp(searchUser(username),successMessage) == 0)
                        {
                            sprintf(tmp, "mensaje de %s: %s", clients[i].username, message);
                            sendMessage(username, tmp);
                        }
                        /* Caso 2: No se encuentra al usuario, se envia un mensaje al cliente autor del mensaje */
                        else
                        {
                        	write(clients[i].out_fd, userNotFoundMessage, MSG_LEN);
                        }
                    }
                    /* Salida del sistema */
                    else if (strcmp(token, ordenSalir) == 0)
                    {
						/* Buscamos a los usuarios que estan hablando con el usuario que cambio su estado */
					    for (j=0; j<N; j++)
					    {
					        if (strcmp(clients[j].actualConversation, clients[i].username) == 0)
					        {
								sprintf(message, "%s se ha desconectado", clients[i].username);
					        	write(clients[j].out_fd, message, MSG_LEN);
					        }
					    }
                        logout(clients[i].username);
                    }
                }
            }
        }
        sleep(1);
    }
}

/* FUNCIONES PARA REALIZAR DISTINTAS TAREAS DEL SERVIDOR */

/*
 * Function:  initialize
 * --------------------
 *
 *  Inicializa el arreglo de clientes
 *
 *  returns: void
 */

void initialize() {
    int i, j;

    for (i=0; i<N; i++) {
        strcpy(clients[i].username, "");
        strcpy(clients[i].status, "");
        clients[i].in_fd = 0;
        clients[i].out_fd = 0;
    }
}


/*
 * Function:  sendMessage
 * --------------------
 *  Enviar mensaje al cliente indicado a través de su pipe
 *
 *  username: usuario al cual se le escribe el mensaje
 *
 *  message: Mensaje a enviar
 *
 *  returns: void
 */
void sendMessage(char username[], char message[])
{
    int i;

    for (i=0; i<N; i++) {
        if (strcmp(clients[i].username, username) == 0) {
            write(clients[i].out_fd, message, strlen(message)+1);
            break;
        }
    }
}

/*
 * Function:  searchUser
 * --------------------
 *  Busca a un usuario en la lista de usuarios del servidor
 *
 *  username: Nombre del usuario a buscar
 *
 *  returns: un mensaje de exito si lo encuentra, y otro sino lo encuentra
 */
const char* searchUser(char username[])
{
    int i;
    char message[MSG_LEN];

    for (i=0; i<N; i++)
    {
        if (strcmp(username,clients[i].username) == 0)
        {
        	return successMessage;
        }
    }
    return userNotFoundMessage;
}

/*
 * Function:  login
 * --------------------
 *  Registra los datos y pipes del usuario
 *
 *  username: Nombre del usuario a registrar
 *
 *  in_fd: Identificador del pipe con el que se leeran datos del cliente
 *
 *  out_fd: Identificador del pipe con el que se enviaran datos al cliente
 *
 *  returns: void
 */
void login(char username[], int in_fd, int out_fd) {
    int i;

    for (i=0; i<N; i++) {
        if (strcmp(clients[i].username, "") == 0)
        {
            strcpy(clients[i].username, username);
            strcpy(clients[i].status, defaultStatus);
            clients[i].in_fd = in_fd;
            clients[i].out_fd = out_fd;
            printf("Login: id: %d username: %s status: %s in_fd:%d out_fd:%d \n",i,clients[i].username,clients[i].status,clients[i].in_fd,clients[i].out_fd);
            break;
        }
    }
    write(out_fd, successMessage, strlen(successMessage));
}

/*
 * Function:  logout
 * --------------------
 *  Eliminar al usuario de todas las listas de amigos y vaciar sus datos
 *
 *  username: Nombre del usuario que se saldra del sistema
 *
 *  returns: void
 */
void logout(char username[])
{
    int i, j, friend_id;
    char in_file_name[11],     	// nombre de los pipes de entrada
             out_file_name[11];

    for(i=0; i<N; i++)
    {
        if (strcmp(clients[i].username, username) == 0)
        {
            strcpy(clients[i].username, "");
            strcpy(clients[i].status, "");
            close(clients[i].in_fd);
            close(clients[i].out_fd);
            clients[i].in_fd = 0;
            clients[i].out_fd = 0;
            unlink(in_file_name);
            unlink(out_file_name);
            break;
        }
    }
}

/*
 * Function:  splitFirst
 * --------------------
 *  Toma la primera palabra de un arreglo
 *
 *  str: string original
 *
 *  first: La primera palabra
 *
 *  second: el resto del arreglo
 *
 *  returns: La primera palabra
 */
char splitFirst(char str[], char first[], char second[])
{
    int i, j, len=strlen(str);

    for (i=0; i<len; i++)
    {
        if (str[i] == ' ') break;
        else first[i] = str[i];
    }

    first[i++] = 0;

    for (j=0; i<len; i++, j++)
        second[j] = str[i];

    second[j] = 0;
    return second[j];
}

/*
 * Function:  printArray
 * --------------------
 *  Imprime un arreglo
 *
 *  arr: El arreglo a imprimir
 *
 *  returns: void
 */
void printArray(int arr[]) {int i; for (i=0; i<N; i++) printf("%d ", arr[i]); printf("\n"); }

/*
 * Function:  sigkillHandler
 * --------------------
 *  Manejador de señales por si el servidor se cierra inesperadamente
 *
 *  signo:
 *
 *  returns: void
 */
static void sigkillHandler(int signo)
{
	printf("Saliendo...");
    unlink(server_pipe_name);	
    sleep(5);
    close(fifo);
    exit(0);
}
