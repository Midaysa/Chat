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
#include "../commons.h"             // strlen

#define MSG_LEN 500
#define STATUS_LEN 100
#define BASIC_PERMISSIONS 0666
#define N 20

// crear contenedor de usuarios
struct client {
    char username[30];
	char status[140];
    char actualConversation[30];
    int in_fd, out_fd;
} clients[N];

void initialize();
void sendMessage(char username[], char message[]);
void write_full(char *token, char dst[]);
const char* searchUser(char username[]);
void login(char username[], int in_fd, int out_fd);
void logout(char username[]);


int main(int argc, char *argv[]) {
    struct timeval tv;          // estructura para indicar el timeout de select
    int n;                      // 1 + file descriptor mas alto
    int rv;                     // return value del select, sirve para saber si
                                // retornó algo útil (>0),
                                // si hubo error (-1) o si hubo timeout (0)
    char message[MSG_LEN];      // string de datos enviados desde los clientes
                                // al servidor y viceversa
    char tmp[MSG_LEN];          // string temporal para agregar origen de mensaje
    int fifo;                   // fd del pipe del servidor
    fd_set fdset, error_fdset;  // crear sets de pipes nominales
    char in_file_name[11],     	// nombre de los pipes de entrada
         out_file_name[11];
    int in_fd, out_fd;          // file descriptors temporales para clientes
    char username[30];          // nombre temporal para clientes
    char server_pipe_name[100]; // nombre del pipe nominal del servidor
    int i, j;                   // variables de iteración simple
    char *token;
    signal(SIGPIPE, SIG_IGN);	// Manejador de senales para SIGPIPE

    if (argc == 2)
    {
        strcpy(server_pipe_name, argv[1]);
    }
    else
    {
        strcpy(server_pipe_name, defaultServer);
    }

    printf(serverStartMessage);

    fifo = open_fifo(server_pipe_name);
    tv.tv_sec = 1;                  // 1 seg de timeout con 0 microsegs
    tv.tv_usec = 0;
    initialize();

    while (true)
    {
        FD_ZERO(&fdset);                // limpiar el set de pipes nominales
        FD_ZERO(&error_fdset);
        FD_SET(fifo, &fdset);           // agregar fifo al set de pipes
        n = 100;                        // sustituir por: fd mas alto +1

        strcpy(message, "");

        for (i=0; i<N; i++)
        {
        	// agregar pipes de todos los usuarios al fd_set
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
        	// existen archivos con datos para leer
            // si existe una nueva solicitud de conexion en el pipe 'fifo'
            if(FD_ISSET(fifo, &fdset))
            {
                // agregar informacion del nuevo usuario al lista
                // leer del pipe fifo los id's de los pipes del cliente

                read(fifo, message, MSG_LEN);
                sscanf(message, "%s %s %s\n", username, out_file_name, in_file_name);
                close(fifo);

                // Si encontramos a un usuario con el mismo nombre rechazamos la conexion
                if (strcmp(searchUser(username),successMessage) == 0)
                {
                	write(out_fd, userNameNotAvaible, strlen(userNameNotAvaible));
                }
                else
                {
                    fifo = open_fifo(server_pipe_name);
                    in_fd = open(in_file_name, O_RDONLY | O_NONBLOCK);
                    out_fd = open(out_file_name, O_WRONLY | O_NONBLOCK);
                    login(username, in_fd, out_fd);
                }


            }

            for (i=0; i<N; i++)
            {
                if (FD_ISSET(clients[i].in_fd, &fdset))
                {
                    printf("comando recibido de |%s|\n", clients[i].username);
                    strcpy(message, "");
                    read(clients[i].in_fd, message, MSG_LEN);
                    printf("message = |%s|\n", message);
                    token = strtok(message, " ");      // token = primera palabra del comando
                    printf("token = |%s|\n", token);

                    if (token == NULL && strcmp(clients[i].username,"") != 0)
                    {
                        printf("null token\n");
                        logout(clients[i].username);
                        continue;
                    }

                    if (strcmp(token, ordenEstoy) == 0)
                    {
						token = strtok(NULL, " ");
						write_full(token, message);
						printf("client status = |%s|\n", message);
						strcpy(clients[i].status, message);

						sprintf(message, "%s ha cambiado de estado a: %s", clients[i].username, clients[i].status);

						// Buscamos a los usuarios que estan hablando con el usuario que cambio su estado
					    for (j=0; j<N; j++)
					    {
					        if (strcmp(clients[j].actualConversation, clients[i].username) == 0)
					        {
					        	write(clients[j].out_fd, message, MSG_LEN);
					        }
					    }

                    }
                    else if (strcmp(token, ordenQuien) == 0)
                    {
                        strcpy(message, "");

                        strcat(message, "Usuarios Conectados \n\n");

                        for (j=0; j<N; j++)
                        {
                            if (strlen(clients[j].username) > 0)
                            {
								printf("j = |%d|\n", j);
								printf("clients[j].username = |%s|\n", clients[j].username);
								strcat(message, clients[j].username);
								strcat(message, ":");
								printf("clients[j].status = |%s|\n", clients[j].status);
								strcat(message, clients[j].status);
								strcat(message, "\n");
                            }
                        }
                        printf("i = |%d|\n", i);
                        write(clients[i].out_fd, message, MSG_LEN);
                    }
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


                    else if (strcmp(token, ordenEscribir) == 0)
                    {
                        token = strtok(NULL, " ");
                        strcpy(username, token);
                        token = strtok(NULL, " ");
                        write_full(token, message);
                        if (strcmp(searchUser(username),successMessage) == 0)
                        {
                            sprintf(tmp, "mensaje de %s: %s", clients[i].username, message);
                            sendMessage(username, tmp);
                        }
                        else
                        {
                        	write(clients[i].out_fd, userNotFoundMessage, MSG_LEN);
                        }

                    }
                    else if (strcmp(token, ordenSalir) == 0)
                    {
                        printf("logging out\n");
                        //write(clients[i].out_fd, "-salir", MSG_LEN);
                        logout(clients[i].username);
                    }
                }
            }
        }
        sleep(1);


    }

}

// FUNCIONES PARA REALIZAR DISTINTAS TAREAS DEL SERVIDOR

// Inicializa el arreglo de clientes
void initialize() {
    int i, j;

    for (i=0; i<N; i++) {
        strcpy(clients[i].username, "");
        strcpy(clients[i].status, "");
        clients[i].in_fd = 0;
        clients[i].out_fd = 0;
    }
}



// Enviar mensaje al cliente indicado a través de su pipe
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

// escribe lo que le sobra a token dentro de dst
void write_full(char *token, char dst[]) {
    char tmp[MSG_LEN] = "";

    while (token != NULL) {
        strcat(tmp, token);
        strcat(tmp, " ");
        token = strtok(NULL, " ");
    }

    strcpy(dst, tmp);
    dst[strlen(dst)-1] = 0;
}

// Busca a un usuario en la lista de usuarios del servidor
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


// Registrar los datos y pipes del usuario
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

// Eliminar al usuario de todas las listas de amigos y vaciar sus datos
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
    printf("%s %s\n",username,LogOutServerMessage);
}


// Toma la primera palabra de un arreglo
char split_first(char str[], char first[], char second[]) {
    int i, j, len=strlen(str);

    for (i=0; i<len; i++) {
        if (str[i] == ' ') break;
        else first[i] = str[i];
    }

    first[i++] = 0;

    for (j=0; i<len; i++, j++)
        second[j] = str[i];

    second[j] = 0;
    return second[j];
}

// Imprime un arreglo
void print_array(int arr[]) {int i; for (i=0; i<N; i++) printf("%d ", arr[i]); printf("\n"); }
