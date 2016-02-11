#include <time.h>               // sleep
#include <fcntl.h>              // O_NONBLOCK, etc
#include <stdio.h>              // printf
#include <sys/queue.h>          // list macros
#include <stdlib.h>             // malloc, free
#include <stdbool.h>            // bool, true, false
#include <string.h>             // strlen
#include <ncurses.h>
#include "../commons.h"         // error messages


#define BASIC_PERMISSIONS 0666
#define MSG_LEN 500
#define ALTO 5 // Alto de la ventana 2
#define LINES_MIN 10 // Alto m�nimo que debe tener el terminal
#define COLS_MIN 25 // Ancho m�nimo que debe tener el terminal
#define TECLA_RETURN 0xD
#define TAM 2048 // Tama�o de buffer

WINDOW *ventana1, *ventana2;

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

/* @Nombre: Quien
 * @Funcion: Esta orden muestra una lista de los usuarios conectados al
 * 			 servidor y el estado de cada uno.
			 La lista desplegada por la orden quien se mostrará en la ventana
			 de conversación.

 * @Entrada: Ninguna
 * @Salida:  Imprime en pantalla
 */
void who(char* out_file_name)

{
	// creamos la orden necesaria para enviarla al servidor servidor
	char* orderToSend = (char *) malloc(strlen(whoOrder));

	// Datos de prueba, estos seran substituidos por los que envie el pipe

	char* prueba_Lista = "Francisco - Estoy triste :( | Pepe - Estoy feliz =D";
	char* prueba_Length = "51";

	// Enviamos la orden al servidor de los datos que necesitamos

	char* buffer; //Variable en la que guardaremos los datos que vamos recibir del pipe
	int length;

	// Recibimos la longitud de los datos que vamos recibir, y
	// reservamos el espacio para recibir el archivo

	length = atoi(prueba_Length);
	buffer = (char *) malloc(length);

	// Recibimos La lista de usuarios

	strcpy(buffer, prueba_Lista);

	// Transformamos la lista

    // Prueba

    char** userName;
    char** userStatus;
    char** userAndStatus;
    int i = 0;

    userName = malloc(2 * sizeof(char *));
    userStatus = malloc(2 * sizeof(char *));
    userAndStatus = malloc(2 * sizeof(char *));

    // Prueba para ver como transformar la lista de usuarios que nos traeria el pipe

    // Hay dos formatos, los dos juntos o el estado y el nombre por su lado, es cuestion de escoger

    printf("Resultado De Funcion Who \n \n");

    for ( i = 0; i <= 1; i = i + 1 )
    {
    	userAndStatus[i] = malloc(strlen(getWord(buffer,"|",i)));
    	userAndStatus[i] = getWord(buffer,"|",i);

        userName[i] = malloc(strlen(getWord(userAndStatus[i],"-",0)));
        userName[i] = getWord(userAndStatus[i],"-",0);

        userStatus[i] = malloc(strlen(getWord(userAndStatus[i],"-",1)));
        userStatus[i] = getWord(userAndStatus[i],"-",1);

        printf("userAndStatus[%d] = %s\n", i,userAndStatus[i]);
        printf("userName[%d] = %s\n", i,userName[i]);
        printf("userStatus[%d] = %s\n", i,userStatus[i]);

    }

    // Liberamos la memoria utilizada para enviar la orden
    free(orderToSend);

}

/* @Nombre: Escribir
 * @Funcion: Esta orden toma el nombre de un usuario conectado como argumento
 * 			 e indica que se quiere conversar con él. Los mensajes enviados
 * 			 después de ejecutar la orden serán dirigidos a ese usuario.
 *
 * 			 Si se vuelve a ejecutar la orden escribir con otro nombre de
 * 			 usuario, los mensajes ahora serán dirigidos al nuevo usuario.
 *
 * 			 De este modo, será posible mantener varias conversaciones con
 * 			 distintos usuarios en la misma pantalla.
 *
 * @Entrada: Cliente cliente: cliente que escribe el mensaje
 * @Entrada: Cliente clienteAEscribir: cliente al que se le escribe el mensaje
 * @Salida:  Imprime en pantalla
 */

void writeTo(char* username,char* userNameToWrite,char* message,int out_fd)

{
	// creamos la orden necesaria para enviarla al servidor servidor
	char* orderToSend = (char *) malloc(strlen(writeToOrder)
			+ strlen("-") + strlen(username)
			+ strlen(">") + strlen(userNameToWrite)
			+ strlen("|") + strlen(message));

	sprintf(orderToSend,"%s-%s>%s|%s",writeToOrder,username,userNameToWrite,message);

	write(out_fd, orderToSend, strlen(orderToSend));

	// Liberamos la memoria utilizada para enviar la orden
	free(orderToSend);
}

/* @Nombre: Estoy
 * @Funcion: Esta orden cambia el estado del usuario.
 * Si un cliente está conversando con determinado usuario
 * (es decir, si la última orden escribir ejecutada en el cliente fue a
 * ese usuario), y este usuario cambia de estado, se debe mostrar
 * una notificación en el cliente con el nuevo estado del usuario.

 * @Entrada: Cliente cliente: cliente que actualiza su estado
 * @Entrada: char* estado: el estado nuevo del cliente
 * @Salida:  Imprime en pantalla
 */

void status(Client* client, char* estado,int out_fd)

{
	// creamos la orden necesaria para enviarla al servidor servidor
	char* orderToSend = (char *) malloc(strlen(whoOrder) + strlen("|") + strlen(client->nombre)
			+ strlen("|") + strlen(estado));
	sprintf(orderToSend,"%s|%s|%s",statusOrder,client->nombre,estado);

	// Enviar al servidor
	write(out_fd, orderToSend, strlen(orderToSend));

	// Recibir mensaje de comfirmacion

	free(orderToSend);

}

/* @Nombre: Salir
 * @Funcion: Esta opción cierra el programa cliente y le notifica al servidor
 * que el usuario se desconectó. Se mostrará un mensaje indicando que el
 * usuario se desconectó en los clientes que estén conversando con él.
 * @Entrada: Cliente cliente que efectua la salida
 * @Salida:  Imprime en pantalla
 */

void logOut(char* userName,char* serverName)

{

	// Creamos la orden necesaria para enviarla al servidor servidor
	char* orderToSend;
	int out_fd;
	orderToSend = (char *) malloc(strlen(logOutOrder) + strlen("|") + strlen(userName));
	out_fd = open(serverName, O_WRONLY);

	sprintf(orderToSend,"%s-%s",whoOrder,userName);

	// Enviar al servidor
	write(out_fd, orderToSend, strlen(orderToSend));
	printf(LogOutMessage,"%s");

	// Liberamos la memoria utilizada para enviar la orden
	close(out_fd);
	free(orderToSend);
	exit(0);
}



int main(int argc, char **argv)
{

    int len;
    char* in_file_name; 		  // nombre de los pipes de entrada
    char* out_file_name;                                // y salida de cada usuario
    int fifo;                                 // pipe "servidor para
                                              // solicitar una nueva conexion
    int in_fd, out_fd;                        // pipes de entrada/salida
    int r;                                    // numero random entre
                                              // 1000000000 y 2000000000-1
    char message[MSG_LEN];

	if (argc == 1)
	{
		in_file_name = (char *) malloc(strlen("System"));
		strcpy(in_file_name,"System");
		out_file_name = (char *) malloc(strlen("/tmp/servidor"));
		strcpy(out_file_name,"/tmp/servidor");
	}

	else if (argc > 1 && argc <= 4)
	{
		// Si recibo un argumento con el prefijo -p entonces asigno un nombre de pipe
			if ((strcmp(argv[1],"-p")) == 0)
			{
				out_file_name = (char *) malloc(strlen(argv[2]));
				strcpy(out_file_name,argv[2]);

				// Si recibo otro argumento entonces defino el nombre de usuario
				if (argc == 4)
				{
					in_file_name = (char *) malloc(strlen(argv[3]));
					strcpy(in_file_name,argv[3]);
				}

				// Si no recibo otro argumento entonces es el usuario del sistema
				else
				{
					in_file_name = (char *) malloc(strlen("System"));
					strcpy(in_file_name,"System");
				}
			}

			// Si no recibo el argumento del pipe
			else
			{
				// Si no defino nombre de servidor, escogo el nombre por defecto
				out_file_name = (char *) malloc(strlen("/tmp/servidor"));
				strcpy(out_file_name,"/tmp/servidor");

				// Recibo un usuario
				if (argc == 2)
				{
					in_file_name = (char *) malloc(strlen(argv[1]));
					strcpy(in_file_name,argv[1]);
				}

				// No recibo ningun dato
				else if (argc == 1)
				{
					in_file_name = (char *) malloc(strlen("System"));
					strcpy(in_file_name,"System");
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

	char * option;



	char c;
	char * order;
	char * userToWrite = NULL;
	int i,j;

	wprintw(ventana1, "Mega Servicio De Chat! Bienvenido! \n \n");

	while(1)
	{
        char buffer[TAM];
        wgetnstr(ventana2, buffer, TAM); // Leer una l�nea de la entrada

//		option = NULL;
//		obtainOption(&option,in_file_name);
//		optionCopy = option;

	   /* get the first token */
		order = getWord(buffer," ",0);


		// Si No Se ha definido el usuario al escribir
		if (userToWrite == NULL)
		{
			wprintw(ventana1, "%s: %s\n", in_file_name,buffer);
		}
		// Si Se ha definido el usuario al escribir
		else
		{
			wprintw(ventana1, "%s -> %s: %s\n", userToWrite,userToWrite,buffer);
		}

		if (strcmp(order,"-quien") == 0)
		{
			who(out_file_name);
		}
		else if (strcmp(order,"-escribir") == 0)
		{
			userToWrite = (char *) malloc(strlen(getWord(buffer," ",1) + 1));
			strcpy(userToWrite, getWord(buffer," ",1));
		}
		else if (strcmp(order,"-estoy") == 0)
		{

		}
		else if (strcmp(order,"-salir") == 0)
		{
			logOut(in_file_name,out_file_name);
		}
		else
		{
			printf("Entrada Invalida, vuelva a intentarlo \n");
		}

        wrefresh(ventana1);
        limpiarVentana2();
//		free(optionCopy);

	}

    srand(time(NULL));                        // inicializa semilla del random

    // abrir pipe publico de conexiones nuevas del servidor
    fifo = open(out_file_name, O_WRONLY);
    
    if (fifo == -1) perror(getErrorMessage(openError,__LINE__,__FILE__));

    // crear pipe (nominal) de entrada
    mkfifo(in_file_name, BASIC_PERMISSIONS, O_NONBLOCK);
    // crear pipe (nominal) de salida
    mkfifo(out_file_name, BASIC_PERMISSIONS | O_NONBLOCK);
    
    write(fifo, message, strlen(message));
    close(fifo);
    // abrir el pipe para leer datos
    in_fd = open(in_file_name, O_RDONLY);
    // abrir el pipe para enviar datos
    out_fd = open(out_file_name, O_WRONLY);


    char mensaje[50];
    len = read(in_fd, mensaje, 50);
    printf("len = %d\n", len);
    
    close(in_fd);
    close(out_fd);
    printf("mensaje = %s.\n", mensaje);




//    while (1)
//    {
//
//
//    }

    // nos aseguramos de que el SO ya escribio el mensaje en el pipe antes de cerrar la app
    sleep(1);
    unlink(in_file_name);
    unlink(out_file_name);
}
