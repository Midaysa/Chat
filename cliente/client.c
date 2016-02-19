/*
 * client.c
 *
 *	Programa que maneja el usuario de manera directa a traves de una interfaz implementada con la libreria ncurses,
 *	el cliente se puede conectar a un servidor y puede enviar mensajes a otros clientes, cambiar su estado y mas
 *
 *  Created on: Jan 18, 2016
 *      Author: francisco y midaysa
 */

#include <time.h>               // sleep
#include <fcntl.h>              // O_NONBLOCK, etc
#include <stdio.h>              // printf
#include <sys/queue.h>          // list macros
#include <sys/select.h>         // select
#include <stdlib.h>             // malloc, free
#include <stdbool.h>            // bool, true, false
#include <string.h>             // strlen
#include <signal.h>             // kill, SIGINT
#include <ncurses.h>
#include "../commons.h"         // Mensajes De Error y funciones comunes

#define BASIC_PERMISSIONS 0666
#define MSG_LEN 500             // NOTA: VER EN CUANTO SE DEJARA ESTE ARREGLO
#define NAME_LEN 50

#define ALTO 5 // Alto de la ventana 2
#define LINES_MIN 10 // Alto m�nimo que debe tener el terminal
#define COLS_MIN 25 // Ancho m�nimo que debe tener el terminal
#define TECLA_RETURN 0xD
#define TAM 2048 // Tama�o de buffer
#define WAIT 1	// Tiempo de espera de la herramienta select

WINDOW *ventana1, *ventana2;

void displayCommandList();
void enfocarVentana2();
void limpiarVentana2();
void writeFull(char *token, char dst[]);
static void sigkill_handler(int signo);

int in_fd;						// pipes de entrada
int out_fd;                     // pipes de salida
char in_file_name[NAME_LEN];
char out_file_name[NAME_LEN];  // nombre de los
							   //pipes de entrada
							   // y salida de cada usuario


/* Hay que permitir que se le pasen argumentos al cliente */
int main(int argc, char *argv[])
{
    int len;
    int fifo;                              // pipe "servidor" para
										   // solicitar una nueva conexion
    char server_pipe_name[NAME_LEN];
    char username[NAME_LEN];
    char command[MSG_LEN];
	char first[MSG_LEN];
	char second[MSG_LEN];
	char message[MSG_LEN];
	char dest[NAME_LEN];
	char*token;
	int alto1; // Alto de la ventana 1

	int    fd_stdin;	// identificador del pipe de stdin
	fd_set readfds;	// fd_set para la entrada de datos
	struct timeval timeoutInput;	// Estructura para definir el timeout
	int    num_readable;	// Variable que indica si hay datos en el pipe de entrada
	signal(SIGINT, sigkill_handler);	// Manejador de senales para SIGINT
	signal(SIGABRT, sigkill_handler);	// Manejador de senales para SIGINT
	signal(SIGTERM, sigkill_handler);	// Manejador de senales para SIGINT


    srand(time(NULL));                         // inicializa semilla del random


	if (argc == 1)
	{
		strcpy(username, defaultUsername);
		strcpy(server_pipe_name, defaultServer);
	}

	else if (argc > 1 && argc <= 4)
	{
		/* Si recibo un argumento con el prefijo -p entonces asigno un nombre de pipe */
			if ((strcmp(argv[1],"-p")) == 0)
			{
				strcpy(server_pipe_name, argv[2]);

				/* Si recibo otro argumento entonces defino el nombre de usuario */
				if (argc == 4)
				{
					strcpy(username, argv[3]);
				}

				/* Si no recibo otro argumento entonces es el usuario del sistema */
				else
				{
					strcpy(username, defaultUsername);
				}
			}

			/* Si no recibo el argumento del pipe */
			else
			{
				/* Si no defino nombre de servidor, escogo el nombre por defecto */
				strcpy(server_pipe_name, defaultServer);

				/* Recibo un usuario */
				if (argc == 2)
				{
			        strcpy(username, argv[1]);
				}

				/* Recibo argumentos pero en el orden incorrecto */
				else
				{
					printf("%s \n",argOrdError);
					exit(0);
				}
			}
	}

	/* Si recibo mas de 4 argumentos hay un error */
	else
	{
		printf("%s", argNumError);
		exit(0);
	}

    initscr(); // Inicializar la biblioteca ncurses

    if (LINES < LINES_MIN || COLS < COLS_MIN) {
        endwin(); // Restaurar la operaci�n del terminal a modo normal
        perror(getErrorMessage(termSizeError,__LINE__,__FILE__));
        exit(0);
    }

    /* Opciones de la biblioteca ncurses */
    cbreak();
    nonl();

    alto1 = LINES - ALTO;
    ventana1 = newwin(alto1, 0, 0, 0); // Crear la ventana 1
    ventana2 = newwin(ALTO, 0, alto1, 0); // Crear la ventana 2
    scrollok(ventana1, TRUE); //Activar el corrimiento autom�tico en la ventana 1
    scrollok(ventana2, TRUE);
    limpiarVentana2(); // Dibujar la l�nea horizontal

    /* Creamos los nombres de los pipes */
    strcpy(in_file_name, username);
    strcpy(out_file_name, username);
    strcat(in_file_name, "_in");
    strcat(out_file_name, "_out");

    /* abrir el pipe publico de conexiones nuevas del servidor */
    fifo = open(server_pipe_name, O_WRONLY);
    if (fifo == -1) perror(getErrorMessage(openError,__LINE__,__FILE__));

    /* crear pipe (nominal) de entrada */
    mkfifo(in_file_name, BASIC_PERMISSIONS, O_NONBLOCK);
    /* crear pipe (nominal) de salida */
    mkfifo(out_file_name, BASIC_PERMISSIONS | O_NONBLOCK);

    /* Se envia al servidor los nombres de los pipes y el nombre de usuario para que pueda registrarlo */
    sprintf(message, "%s %s %s\n", username, in_file_name, out_file_name);
    wprintw(ventana1,"message = %s", message);
    wrefresh(ventana1);
    write(fifo, message, MSG_LEN);
    close(fifo);


    strcpy(message, "");

    in_fd = open(in_file_name, O_RDONLY);      // abrir el pipe para leer datos
    if (in_fd == -1) perror(getErrorMessage(openError,__LINE__,__FILE__));

    /* Esperamos y recibimos la respuesta del servidor */
    read(in_fd, message, MSG_LEN);
    close(in_fd);
    wprintw(ventana1,"%s %s \n", loginResultMessage,message);
    wrefresh(ventana1);

    /* Si el nombre del usuario ya se encuentra en uso en el sistema se cierra el programa */
    if (strcmp(message,userNameNotAvaible) == 0)
	{
        close(in_fd);
        close(out_fd);
        unlink(out_file_name);
        unlink(in_file_name);
        endwin(); // Restaurar la operaci�n del terminal a modo normal
        printf("%s\n",userNameNotAvaible);
        exit(0);
	}

    strcpy(dest, "");

	wprintw(ventana1, "%s\n" ,welcomeMessage);
	wrefresh(ventana1);


	fd_stdin = fileno(stdin);

	/* Ciclo Principal */
	while(true)
	{
		/* Abrimos el pipe de lectura de manera no bloqueante */
        in_fd = open(in_file_name, O_RDONLY | O_NONBLOCK);      // abrir el pipe para leer datos
        if (in_fd == -1) perror(getErrorMessage(openError,__LINE__,__FILE__));

        /* Recibimos data del servidor y la imprimimos */
        strcpy(message, "");
        read(in_fd, message, MSG_LEN);

        if (strcmp(message, "") != 0)
        {
        	wprintw(ventana1,"%s\n", message);
        }

        wrefresh(ventana1);

        out_fd = open(out_file_name, O_WRONLY);  // abrir el pipe para enviar datos
        if (out_fd == -1) perror(getErrorMessage(openError,__LINE__,__FILE__));
        enfocarVentana2();


        FD_ZERO(&readfds);
        FD_SET(fd_stdin, &readfds);

        /* Definimos la espera del chequeo de los pipes */
        timeoutInput.tv_sec = WAIT;    // WAIT segundos
        timeoutInput.tv_usec = 0;    // 0 millisegundos

        enfocarVentana2();
        num_readable = select(fd_stdin + 1, &readfds, NULL, NULL, &timeoutInput);

        if (num_readable == -1)
        {
			perror(getErrorMessage(selectError,__LINE__, __FILE__));
			exit(1);
		}
        else if (num_readable == 0)
        {
			close(out_fd);
			wrefresh(ventana1);
			continue;
		}
		else
		{
			/* Pedimos la entrada del usuario */
			wgetnstr(ventana2, command, MSG_LEN); // Leer una l�nea de la entrada

			/* Permitimos el enter para evitar un segmentation fault */
			if (strcmp(command,"") == 0)
			{
				wrefresh(ventana1);
				limpiarVentana2();
				continue;
			}

			command[strlen(command)] = 0;          // sustituir \n por \0 al final

			token = strtok(command, " ");      // token = primera palabra del comando
			wrefresh(ventana1);

			/* Caso 1: Commando */

			if (token[0] == '-')
			{
				wprintw(ventana1, "%s: %s\n", in_file_name, command);
				/* Caso 1.1: Cambiar Estado */
				if (strcmp(token, ordenEstoy) == 0)
				{
					writeFull(token, command);
					write(out_fd, command, MSG_LEN);
					// mostrar en algun label de la GUI este estado
				}

				/* Caso 1.2: Pedir a servidor la lista de usuarios */
				else if (strcmp(token, ordenQuien) == 0)
				{
					write(out_fd, command, MSG_LEN);
				}

				/* Caso 1.3: Cambiar conversacion */
				else if (strcmp(token, ordenEscribir) == 0)
				{
					/* extraer destinatario y pegarlo en dest */
					token = strtok(NULL, " ");
					strcpy(dest, token);

					if (strcmp(dest, username) == 0)
					{
						wprintw(ventana1, "%s\n" ,writeToMySelf);
					}

					else
					{
						sprintf(command, "%s %s", ordenCambiarConversacion,dest);
						write(out_fd, command, MSG_LEN);
					}
				}

				/* Caso 1.4: Cierre de Sesion */
				else if (strcmp(token, ordenSalir) == 0)
				{
					write(out_fd, command, MSG_LEN);
					sleep(1);
					break;
				}

				/* Caso 1.5: Cierre de Sesion */
				else if (strcmp(token, ordenAyuda) == 0)
				{
					displayCommandList();
				}

				/* Caso 1.6: Orden Invalida */
				else
				{
					wprintw(ventana1, "%s\n",ordenInvalida);
				}

			}

			/* Caso 2: Mensaje a enviar */
			else
			{
				/* Si No Se ha definido el usuario al escribir */
				if (dest == "")
				{
					wprintw(ventana1, "%s: %s\n", in_file_name, command);
				}
				/* Si Se ha definido el usuario al escribir */
				else
				{
					wprintw(ventana1, "%s -> %s: %s\n", username, dest, command);
				}

				/* Si no hay conversacion selecionada rechazamos el mensaje */
				if (strcmp(dest,"") == 0)
				{
					wprintw(ventana1, noUserSelectedMessage);
				}
				/* Si hay conversacion seleccionada enviamos el mensaje */
				else
				{
					sprintf(message, "%s %s ",ordenEscribir, dest);
					writeFull(token, command);
					strcat(message, command);
					write(out_fd, message, MSG_LEN);
				}
			}
			close(out_fd);

			wrefresh(ventana1);
			limpiarVentana2();
		}
	}

    close(out_fd);
    sleep(5);
    unlink(out_file_name);
    close(in_fd);
    unlink(in_file_name);
    endwin(); // Restaurar la operaci�n del terminal a modo normal
    exit(0);
}


/*

/*
 * Function:  enfocarVentana2
 * --------------------
 *  Mueve el cursor al punto de inserci�n actual de la ventana 2.
 *
 *  returns: void
 */
void enfocarVentana2() {
    int y, x;
    getyx(ventana2, y, x);
    wmove(ventana2, y, x);
    wrefresh(ventana2);
}

/*
 */

/*
 * Function:  limpiarVentana2
 * --------------------
 *  Borra el contenido de la ventana 2 y ubica el cursor en la esquina
 *  superior izquierda de esta ventana.
 *
 *  returns: void
 */
void limpiarVentana2() {
    wclear(ventana2);
    mvwhline(ventana2, 0, 0, 0, 20); // Dibujar la l�nea horizontal
    wmove(ventana2, 1, 0);
    wrefresh(ventana2);
}

/*
 * Function:  displayCommandList
 * --------------------
 *  Muestra la lista de comandos disponibles para el cliente
 *
 *  returns: void
 */
void displayCommandList()
{
	wprintw(ventana1, helpMenu);
	wrefresh(ventana2);
}

/*
 * Function:  sigkillHandler
 * --------------------
 *  Manejador de señales por si el cliente se cierra inesperadamente
 *
 *  signo:
 *
 *  returns: void
 */
static void sigkill_handler(int signo) {
	wprintw(ventana2, "Cerrando Aplicación. Por favor espere.\n");
    write(out_fd, ordenSalir, MSG_LEN);
	close(out_fd);
    sleep(5);
    unlink(out_file_name);
    close(in_fd);
    unlink(in_file_name);
    endwin(); // Restaurar la operaci�n del terminal a modo normal
    exit(0);
}
