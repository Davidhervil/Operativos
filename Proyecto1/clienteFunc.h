#ifndef clienteFunc_h
#define clienteFunc_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

#define ALTO 5 // Alto de la ventana 2
#define LINES_MIN 10 // Alto mínimo que debe tener el terminal
#define COLS_MIN 25 // Ancho mínimo que debe tener el terminal
#define TECLA_RETURN 0xD
#define TAM 2048 // Tamaño de buffer

char* concat(char *s1, char *s2);
char* crearPipe_w(char* usuario);
char* crearPipe_r(char* usuario);
int conectarServidor(char * usuario,char * pipe_serv);
char * obtener_usr_displ(char * bffr);
void end(int fdr,int fdw,char * pipe1, char *pipe2);
void salirbien(int signum);
void enfocarVentana2(WINDOW *ventana2);
void limpiarVentana2(WINDOW *ventana2);

#endif