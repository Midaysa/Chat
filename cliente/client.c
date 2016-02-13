#include <time.h>               // sleep
#include <fcntl.h>              // O_NONBLOCK, etc
#include <stdio.h>              // printf
#include <sys/queue.h>          // list macros
#include <stdlib.h>             // malloc, free
#include <stdbool.h>            // bool, true, false
#include <string.h>             // strlen
#include <signal.h>             // kill, SIGINT

#define BASIC_PERMISSIONS 0666
#define MSG_LEN 500             // NOTA: VER EN CUANTO SE DEJARA ESTE ARREGLO
#define NAME_LEN 50


void ChildProcess(char[]);
void ParentProcess(char[]);
void write_full(char *token, char dst[]);
char split_first(char[], char[], char[]);


/* Hay que permitir que se le pasen argumentos al cliente */
int main(int argc, char *argv[]) {
    int len;
    char in_file_name[NAME_LEN], out_file_name[NAME_LEN];  // nombre de los
                                               //pipes de entrada
                                               // y salida de cada usuario
    int fifo;                                  // pipe "servidor" para
                                               // solicitar una nueva conexion
    int in_fd, out_fd;                         // pipes de entrada/salida
    int r;                                     // numero random entre
                                               // 1000000000 y 2000000000-1
    char message[MSG_LEN];
    char server_pipe_name[NAME_LEN];
    char username[NAME_LEN];

    srand(time(NULL));                         // inicializa semilla del random

    if (argc == 2) {
        strcpy(server_pipe_name, "/tmp/servidor");
        strcpy(username, argv[1]);
    }
    else {
        strcpy(server_pipe_name, argv[2]);
        strcpy(username, argv[3]);
    }

    //printf("%s %s\n", server_pipe_name, username);

    // abrir el pipe publico de conexiones nuevas del servidor
    fifo = open(server_pipe_name, O_WRONLY);
    printf("%s\n", server_pipe_name);

    if (fifo == -1) perror("open(server_pipe_name)");

    // in_file_name = username + "_in"
    // out_file_name = username + "_out"
    strcpy(in_file_name, username);
    strcpy(out_file_name, username);
    strcat(in_file_name, "_in");
    strcat(out_file_name, "_out");

    // message = in_file_name + ' ' + out_file_name
    sprintf(message, "%s %s %s\n", username, in_file_name, out_file_name);

    // crear pipe (nominal) de entrada
    mkfifo(in_file_name, BASIC_PERMISSIONS, O_NONBLOCK);
    // crear pipe (nominal) de salida
    mkfifo(out_file_name, BASIC_PERMISSIONS | O_NONBLOCK);

    write(fifo, message, strlen(message));
    close(fifo);

    if (fork() == 0)
        ChildProcess(in_file_name);     // Proceso que escucha notificaciones
    else
        ParentProcess(out_file_name);   // Proceso que manda mensajes
}

// Maneja la salida de los mensajes
void ParentProcess(char out_file_name[]) {
    int out_fd;
    bool is_writing = false;
    char command[MSG_LEN], first[MSG_LEN], second[MSG_LEN], message[MSG_LEN],
         dest[NAME_LEN], *token;

    while(true) {
        out_fd = open(out_file_name, O_WRONLY);  // abrir el pipe para enviar datos
        strcpy(command, "");
        token = NULL;
        fgets(command, MSG_LEN, stdin);    // leer comando del usuario
        command[strlen(command)-1] = 0;    // sustituir \n por \0 al final
        token = strtok(command, " ");      // token = primera palabra del comando
        printf("command = |%s| token = |%s|\n", command, token);

        if (strcmp(token, "-estoy") == 0) {
            is_writing = false;
            write_full(token, command);
            printf("command = |%s|\n", command);
            write(out_fd, command, MSG_LEN);
            // mostrar en algun label de la GUI este estado
        }
        else if (strcmp(token, "-quien") == 0) {
            is_writing = false;
            write(out_fd, command, MSG_LEN);
        }
        else if (strcmp(token, "-escribir") == 0) {
            is_writing = true;
            // extraer destinatario y pegarlo en dest
            token = strtok(NULL, " ");
            strcpy(dest, token);
        }
        else if (strcmp(token, "-salir") == 0) {
            is_writing = false;
            write(out_fd, command, MSG_LEN);
            sleep(1);
            break;
        }
        else if (is_writing){
            sprintf(message, "-escribir %s ", dest);
            write_full(token, command);
            strcat(message, command);
            write(out_fd, message, MSG_LEN);
        }
        close(out_fd);
    }
    
    close(out_fd);
    unlink(out_file_name);
    printf("ParentProcess exiting\n");
    sleep(2);
    exit(0);
}

void ChildProcess(char in_file_name[]) {
    char message[MSG_LEN];
    int in_fd, len;

    while(true) {
        in_fd = open(in_file_name, O_RDONLY);      // abrir el pipe para leer datos
        strcpy(message, "");
        read(in_fd, message, MSG_LEN);
        close(in_fd);
        printf("Respuesta del servidor: %s\n", message);

        if (strcmp(message, "-salir") == 0) {
            break;
        }
        sleep(1);
    }

    close(in_fd);
    unlink(in_file_name);
    printf("ChildProcess exiting\n");
    sleep(2);
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
