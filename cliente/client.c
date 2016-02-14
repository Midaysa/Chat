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
#include "../commons.h"         // error messages

#define BASIC_PERMISSIONS 0666
#define MSG_LEN 500             // NOTA: VER EN CUANTO SE DEJARA ESTE ARREGLO
#define NAME_LEN 50

#define ALTO 5 // Alto de la ventana 2
#define LINES_MIN 10 // Alto m�nimo que debe tener el terminal
#define COLS_MIN 25 // Ancho m�nimo que debe tener el terminal
#define TECLA_RETURN 0xD
#define TAM 2048 // Tama�o de buffer
#define WAIT 3

WINDOW *ventana1, *ventana2;

void enfocarVentana2();
void limpiarVentana2();
void write_full(char *token, char dst[]);
void sigintHandler(int dummy);
void AlrmSigHnd(int signo);


/* Hay que permitir que se le pasen argumentos al cliente */
int main(int argc, char *argv[])
{
    int len;
    char in_file_name[NAME_LEN], out_file_name[NAME_LEN];  // nombre de los
                                               //pipes de entrada
                                               // y salida de cada usuario
    int fifo;                                  // pipe "servidor" para
                                               // solicitar una nueva conexion
    int in_fd, out_fd;                         // pipes de entrada/salida
    int r;                                     // numero random entre
                                               // 1000000000 y 2000000000-1
    char server_pipe_name[NAME_LEN];
    char username[NAME_LEN];
    char command[MSG_LEN], first[MSG_LEN], second[MSG_LEN], message[MSG_LEN],
             dest[NAME_LEN], *token;


    srand(time(NULL));                         // inicializa semilla del random


	if (argc == 1)
	{
		strcpy(username, "System");
		strcpy(server_pipe_name, "/tmp/servidor");
	}

	else if (argc > 1 && argc <= 4)
	{
		// Si recibo un argumento con el prefijo -p entonces asigno un nombre de pipe
			if ((strcmp(argv[1],"-p")) == 0)
			{
				strcpy(server_pipe_name, argv[2]);

				// Si recibo otro argumento entonces defino el nombre de usuario
				if (argc == 4)
				{
					strcpy(username, argv[3]);
				}

				// Si no recibo otro argumento entonces es el usuario del sistema
				else
				{
					strcpy(username, "System");
				}
			}

			// Si no recibo el argumento del pipe
			else
			{
				// Si no defino nombre de servidor, escogo el nombre por defecto
				strcpy(server_pipe_name, "/tmp/servidor");

				// Recibo un usuario
				if (argc == 2)
				{
			        strcpy(username, argv[1]);
				}

				// Recibo argumentos pero en el orden incorrecto
				else
				{
					printf("%s \n",argOrdError);
					exit(0);
				}
			}
	}

    // Si recibo mas de 4 argumentos hay un error
	else
	{
		printf("%s", argNumError);
		exit(0);
	}

    initscr(); // Inicializar la biblioteca ncurses

    if (LINES < LINES_MIN || COLS < COLS_MIN) {
        endwin(); // Restaurar la operaci�n del terminal a modo normal
        printf("El terminal es muy peque�o para correr este programa.\n");
        exit(0);
    }

    // Opciones de la biblioteca ncurses
    cbreak();
    nonl();

    int alto1 = LINES - ALTO; // Alto de la ventana 1
    ventana1 = newwin(alto1, 0, 0, 0); // Crear la ventana 1
    ventana2 = newwin(ALTO, 0, alto1, 0); // Crear la ventana 2
    scrollok(ventana1, TRUE); //Activar el corrimiento autom�tico en la ventana 1
    scrollok(ventana2, TRUE);
    limpiarVentana2(); // Dibujar la l�nea horizontal

    strcpy(in_file_name, username);
    strcpy(out_file_name, username);
    strcat(in_file_name, "_in");
    strcat(out_file_name, "_out");

    // message = in_file_name + ' ' + out_file_name
    wprintw(ventana1,"username = %s, in_file_name = %s, out_file_name = %s\n", username, in_file_name, out_file_name);
    wrefresh(ventana1);

    // abrir el pipe publico de conexiones nuevas del servidor
    fifo = open(server_pipe_name, O_WRONLY);
    wprintw(ventana1,"server_pipe_name = %s\n", server_pipe_name);
    wrefresh(ventana1);

    if (fifo == -1) perror("open(server_pipe_name)");

    // crear pipe (nominal) de entrada
    mkfifo(in_file_name, BASIC_PERMISSIONS, O_NONBLOCK);
    // crear pipe (nominal) de salida
    mkfifo(out_file_name, BASIC_PERMISSIONS | O_NONBLOCK);



    sprintf(message, "%s %s %s\n", username, in_file_name, out_file_name);
    wprintw(ventana1,"message = %s\n", message);
    write(fifo, message, strlen(message));
    close(fifo);


    strcpy(message, "");
    in_fd = open(in_file_name, O_RDONLY);      // abrir el pipe para leer datos
    read(in_fd, message, MSG_LEN);
    close(in_fd);
    wprintw(ventana1,"Respuesta del servidor: %s \n", message);
    wrefresh(ventana1);

    if (strcmp(message,userNameNotAvaible) == 0)
	{
        unlink(out_file_name);
        unlink(in_file_name);
        endwin(); // Restaurar la operaci�n del terminal a modo normal
        printf("%s\n",userNameNotAvaible);
        exit(0);
	}

    strcpy(dest, "");

	wprintw(ventana1, "--------- Mega Servicio De Chat! Bienvenido! ---------\n \n");
	wrefresh(ventana1);

	int    fd_stdin;
	fd_set readfds;
	fd_stdin = fileno(stdin);
	struct timeval timeoutInput;
	int    num_readable;


	while(true)
	{
        char buffer[TAM];

        in_fd = open(in_file_name, O_RDONLY | O_NONBLOCK);      // abrir el pipe para leer datos
        strcpy(message, "");
        read(in_fd, message, MSG_LEN);
        //close(in_fd);
        if (strcmp(message, "") != 0)
        {
        	wprintw(ventana1,"Respuesta del servidor:\n\n%s\n", message);
        }

        wrefresh(ventana1);


        out_fd = open(out_file_name, O_WRONLY);  // abrir el pipe para enviar datos
        enfocarVentana2();


        FD_ZERO(&readfds);
        FD_SET(fd_stdin, &readfds);

        /* Waiting for some seconds */
        timeoutInput.tv_sec = WAIT;    // WAIT seconds
        timeoutInput.tv_usec = 0;    // 0 milliseconds

        enfocarVentana2();
        num_readable = select(fd_stdin + 1, &readfds, NULL, NULL, &timeoutInput);

        wprintw(ventana1, "%d\n", num_readable);

        if (num_readable == -1)
        {

			fprintf(stderr, "\nError in select : %s\n", getErrorMessage(selectError,__LINE__, __FILE__));
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
			wgetnstr(ventana2, command, MSG_LEN); // Leer una l�nea de la entrada

			command[strlen(command)] = 0;          // sustituir \n por \0 al final
			token = strtok(command, " ");      // token = primera palabra del comando
			wprintw(ventana1,"command = |%s| token = |%s|\n", command, token);
			wrefresh(ventana1);

			// Si No Se ha definido el usuario al escribir
			if (dest == "")
			{
				wprintw(ventana1, "%s: %s\n", in_file_name,command);
			}
			// Si Se ha definido el usuario al escribir
			else
			{
				wprintw(ventana1, "%s -> %s: %s\n", username,dest,command);
			}

			//

			if (token[0] == '-')
			{
				if (strcmp(token, "-estoy") == 0) {
					write_full(token, command);
					printf("command = |%s|\n", command);
					write(out_fd, command, MSG_LEN);
					// mostrar en algun label de la GUI este estado
				}
				else if (strcmp(token, "-quien") == 0) {
					write(out_fd, command, MSG_LEN);
				}
				else if (strcmp(token, "-escribir") == 0) {
					// extraer destinatario y pegarlo en dest
					token = strtok(NULL, " ");
					strcpy(dest, token);
					sprintf(command, "-cambiarConversacion %s ", dest);
					write(out_fd, command, MSG_LEN);
				}
				else if (strcmp(token, "-salir") == 0) {
					write(out_fd, command, MSG_LEN);
					sleep(1);
					break;
				}
				else
				{
					wprintw(ventana1, "Orden Invalida\n");
				}
			}
			else
			{
				if (strcmp(dest,"") == 0)
				{
					wprintw(ventana1, "No le esta escribiendo a ningun usuario!\n");
				}
				else
				{
					sprintf(message, "-escribir %s ", dest);
					write_full(token, command);
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



void write_full(char *token, char dst[]) {
    char tmp[MSG_LEN] = "";

    while (token != NULL) {
        //printf("write_full :: %s\n", token);
        strcat(tmp, token);
        strcat(tmp, " ");
        token = strtok(NULL, " ");
    }
    strcpy(dst, tmp);
    dst[strlen(dst)-1] = 0;
}

/* Mueve el cursor al punto de inserci�n actual de la ventana 2. */
void enfocarVentana2() {
    int y, x;
    getyx(ventana2, y, x);
    wmove(ventana2, y, x);
    wrefresh(ventana2);
}

/* Borra el contenido de la ventana 2 y ubica el cursor en la esquina
 * superior izquierda de esta ventana.
 */
void limpiarVentana2() {
    wclear(ventana2);
    mvwhline(ventana2, 0, 0, 0, 20); // Dibujar la l�nea horizontal
    wmove(ventana2, 1, 0);
    wrefresh(ventana2);
}

void sigintHandler(int dummy)
{
    endwin(); // Restaurar la operaci�n del terminal a modo normal
    exit(0);
}

