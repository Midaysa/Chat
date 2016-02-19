/*
 * commons.c
 *
 *	Contiene los mensajes que se utilizan en el programa y ademas funciones comunes para el cliente
 *	y el servidor
 *
 *  Created on: Jan 20, 2016
 *      Author: francisco y midaysa
 */

#include <stdlib.h>
#include <string.h>             // strlen
#include <stdio.h>              // printf
#include <time.h>               // sleep
#include <fcntl.h>              // O_NONBLOCK, etc
#include <string.h>             // strlen
#include <sys/stat.h>           // mkfifo

#define BASIC_PERMISSIONS 0666
#define MSG_LEN 500

// Mensajes de error

const char *mkfifoError = "mkfifo Error";
const char *selectError = "select Error";
const char *openError = "open Error";
const char *argNumError = "Numero Incorrecto de Argumentos";
const char *argOrdError = "Orden Incorrecto de Argumentos";
const char *termSizeError = "El terminal es muy pequeno para correr este programa.\n";
const char *rvError = "rvError ";
const char *writeToMySelf = "No puedes escribir mensajes para ti mismo";


// Mensajes de sistema

const char *LogOutMessage = "Cerrando Sesion... Thank You For Using Our Chat Services!";
const char *LogOutServerMessage = " ha cerrado sesion!";
const char *noUserSelectedMessage = "No le esta escribiendo a ningun usuario!\n";
const char *defaultStatus = "Sin Status";
const char *defaultServer = "/tmp/servidor";
const char *defaultUsername = "System";
const char *loginResultMessage = "Resultado Del Inicio De Sesion:";
const char *helpMenu = "-quien: Muestra una lista de los usuarios conectados \n -estoy <Nuevo estado>: Cambia el estado actual \n -escribir <Usuario a escribir>: Cambia el usuario a escribir \n -salir: Cierra Sesion \n -ayuda: Muestra menu de commandos \n ";

// Ordenes Cliente->Servidor

const char *ordenQuien = "-quien";
const char *ordenEscribir = "-escribir";
const char *ordenEstoy = "-estoy";
const char *ordenSalir = "-salir";
const char *ordenAyuda = "-ayuda";
const char *ordenCambiarConversacion = "-cambiarConversacion";
const char *ordenInvalida = "Orden Invalida";

const char *serverStartMessage = "Iniciando servidor!\n";
const char *welcomeMessage = "--------- Mega Servicio De Chat! Bienvenido! ---------\n \n";
const char *successMessage = "Operacion Exitosa";
const char *userNotFoundMessage = "El usuario al que quiere escribirle no se encuentra conectado";
const char *userNameNotAvaible = "El nombre de usuario no se encuentra disponible, por favor escoja otro";

char* getErrorMessage(const char* errorMessage,int line, char* file)

{
	// Convertimos el numero de linea en un String

	char* lineString;
	lineString = (char *) malloc(sizeof(line));
	sprintf(lineString, "%d", line);

	// Inicializamos la variable en la que devolveremos el error

	char* finalMessage;
	finalMessage = (char *) malloc(strlen(errorMessage) + strlen(" at line ")
			+ strlen(lineString) + strlen(" in file ") + strlen(file));

	// Iniciamos el mensaje con el error

	finalMessage = strcpy(finalMessage, errorMessage);

	// Agregamos la linea en la que ocurrio

	strcat(finalMessage, " at line ");
	strcat(finalMessage, lineString);

	// Agregamos el archivo donde ocurrio

	strcat(finalMessage, " in file ");
	strcat(finalMessage, file);

	// Retornamos el mensaje final

	return finalMessage;

}
// Separa un string con el delimitador seleccionado y devuelve la palabra seleccionada con index
char* getWord(char* string,char* delimeter,int index)

{
	// Si la lista es nula entonces abandonamos la funcion

	if (string == NULL)
	{
		return NULL;
	}

	char* stringCopy; //Copia del string original para no cambiarlo
	char* word; // Palabra a ser obtenida
	int i; // Iterador
	char** stringCopyPointer; // Apuntador a la copia del string necesaria para la funcion strsep
	char* stringCopyAddress;
	char* stringCopyPointerAddress;

	// Reservamos la memoria para la copia del string

	stringCopy = (char *) malloc(strlen(string));
	stringCopyAddress = stringCopy;
	strcpy(stringCopy, string);
	stringCopyPointer  = &stringCopy;
	word = (char *) malloc(strlen(string));

	// Recorremos la lista de palabras obteniendo sus palabras una a una

	for ( i = 0; i <= index; i = i + 1 )
	{
		strcpy(word, strsep(stringCopyPointer,delimeter));

		// Si la palabra es igual al string completo, nos salimos de la funcion

		if(stringCopy == NULL)
		{
			return word;
		}

	}

	// Si no ocurrieron ninguno de los casos anteriores entonces debimos obtener la palabra deseada

	free(stringCopyAddress);
	return word;
}


/*
 * Function:  openFifo
 * --------------------
 *  crea y abre el pipe nominal fifo_name retorna el file descriptor del pipe creado
 *
 *  fifo_name: nombre del pipe nominal
 *
 *  returns: void
 */
int openFifo(const char *fifo_name)
{
    int fifo;

    // eliminar el pipe nominal creado en alguna otra ejecución del server
    unlink(fifo_name);
    // esperar 1 seg para que el SO lo elimine completamente
    sleep(1);
    // crear pipe (nominal) de conexiones nuevas
    mkfifo(fifo_name, BASIC_PERMISSIONS | O_NONBLOCK);
    // abrir el pipe para leer conexiones entrantes
    fifo = open(fifo_name, O_RDONLY | O_NONBLOCK);
    if (fifo == -1) perror("server mkfifo");

    return fifo;
}

/*
 * Function:  writeFull
 * --------------------
 *  Escribe lo que le sobra a token dentro de dst
 *
 *  token: String original
 *
 *  dst: String destino
 *
 *  returns: void
 */
void writeFull(char *token, char dst[])
{
    char tmp[MSG_LEN] = "";

    while (token != NULL)
    {
        strcat(tmp, token);
        strcat(tmp, " ");
        token = strtok(NULL, " ");
    }

    strcpy(dst, tmp);
    dst[strlen(dst)-1] = 0;
}
