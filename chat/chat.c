// chat.c
// Prototipo de interfaz para el programa de chat.
//
// Autor: Carlos G�mez  2008-06-23
// 
// Este programa est� basado en la biblioteca ncurses, que permite realizar 
// aplicaciones basadas en varias ventanas en el terminal. El programa divide 
// la pantalla en dos ventanas, la primera para desplegar la conversaci�n y la 
// segunda para la entrada del usuario.
// 
// Para imprimir en una ventana, se puede utilizar la funci�n wprintw de 
// ncurses, y para ver el resultado de la impresi�n se debe llamar a wrefresh.
// Por ejemplo:
// 
// wprintw(ventana1, "Bienvenido al chat");
// wrefresh(ventana1);
// 
// Despu�s de imprimir en la ventana 1, usualmente ser� necesario llamar a 
// enfocarVentana2, con el fin de restaurar la posici�n del cursor a la 
// posici�n que ten�a en la ventana 2.
//
// Puede obtener m�s informaci�n sobre ncurses en
// 
// man ncurses
// NCURSES Programming HOWTO. http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/
//

#include <ncurses.h>
#include <stdlib.h>
#include "chat.h"

#define ALTO 5 // Alto de la ventana 2
#define LINES_MIN 10 // Alto m�nimo que debe tener el terminal
#define COLS_MIN 25 // Ancho m�nimo que debe tener el terminal
#define TECLA_RETURN 0xD
#define TAM 2048 // Tama�o de buffer

WINDOW *ventana1, *ventana2;

int main() {
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

    while(1) {
        char buffer[TAM];
        wgetnstr(ventana2, buffer, TAM); // Leer una l�nea de la entrada
        if (strcmp(buffer, "-salir") == 0) {
            break;
        }
        wprintw(ventana1, "carlos: %s\n", buffer);
        wrefresh(ventana1);
        limpiarVentana2();
    }

    endwin(); // Restaurar la operaci�n del terminal a modo normal
    exit(0);
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
