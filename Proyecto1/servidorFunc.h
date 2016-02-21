#ifndef servidorFunc_h
#define servidorFunc_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#define TAM_BUFFER 2048
#define MAX_USR 20

char * pipe_com;
typedef struct usuarios{
	int fd_lectura;
	int fd_escritura;
	char nombre[20]; 
	char estado[1024];
	char nombre_destino[20];
}usuario;

void inicializar(usuario U[]);
char * obtener_usuario(char * buffer);
int esta(usuario U[],char * usr);
char * obtener_pipe_lect(char * usr);
char * obtener_pipe_escr(char * usr);
int anhadir_usuario(usuario conjunto[], char * usr, int fdr, int fdw);
void eliminar_usuario(usuario U[], int pos);
int calcular_cheq(usuario conected[]);
int procesar(char * buffer, usuario U[], int pos);
void explotar(int signum);

#endif
