#include "clienteFunc.h"
/* Mueve el cursor al punto de inserción actual de la ventana 2. */
// Funcion para concatenar.
char* concat(char *s1, char *s2){
	/*	Esta funcion ocncatena dos strings y retorna n apuntador a caracteres con la concatenacion
		
		VARIABLES:
			result 	: apuntador a caracteres a retornar con la concatenaicon
			s1 		: primer string a concatenar
			s2 		: segundo string a concatenar
	*/
	char *result;
    result = (char*)malloc(strlen(s1)+strlen(s2)+1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

//Crear pipe de wscritura.
char* crearPipe_w(char* usuario){
	/*	Esta funcion recibe un apuntador a caracteres con el nombre de usuario asociado al cliente
		y contsruye el pipe de escritura y retorna un apuntador a caracteres con el path del pipe
			
		VARIABLES:
			usuario 	: nombre del usuario
			dir_w 		: constante auxiliar para la primera parte del path del pipe
			escritura 	: apuntador a retornar con el path del pipe de escritura
	*/
	char* dir_w = "/tmp/w_";
    char* escritura = concat(dir_w,usuario);
    if(unlink(escritura)>=0){
		printf("Unlink hechor\n");
	}
    if(mkfifo(escritura, 0666)<0){
    	printf("No se creo el pipe");
    }
    return escritura;
}

//Crear pipe de lectura.
char* crearPipe_r(char* usuario){
	/*	Esta funcion recibe un apuntador a caracteres con el nombre de usuario asociado al cliente
		y contsruye el pipe de lectura y retorna un apuntador a caracteres con el path del pipe
			
		VARIABLES:
			usuario 	: nombre del usuario
			dir_r 		: constante auxiliar para la primera parte del path del pipe
			lectura 	: apuntador a retornar con el path del pipe de lectura
	*/
	char* dir_l = "/tmp/r_";
	char* lectura = concat(dir_l,usuario);
	if(unlink(lectura)>=0){
		printf("Unlink hechor\n");
	}
    if(mkfifo(lectura, 0666)<0){
    	printf("No se creo el pipe");
    }
    return lectura;
}

//Conectarse al servidor (Enviar nombre del usuario por el pipe com)
int conectarServidor(char * usuario,char * pipe_serv){
	/*	Esta funcion recibe el nombre de usuario asoaciado al cliente, y el pipe de comunicacion indicado
		al llamar al cliente, en caso de no ser indicado sera el pipe establecido por defecto. 
	
		VARIABLES:
			usuario 	: nombre del usuario
			pipe_serv 	: nombre del pipe del servidor
			fd 			: descriptor de archivo de la llamada open del pipe del servidor
	*/
	int fd;
	if((fd = open(pipe_serv, O_WRONLY |O_NONBLOCK))<0){
		fprintf(stderr, "Error al abrir pipe de comunicacion.\n");
		return 0;
	}	
    if(write(fd, usuario,TAM)<=0){
    	fprintf(stderr, "Error al escribir en pipe de comunicacion. Descriptor %d\n",fd);
		return 0;
    }
    close(fd);
    return 1;
}

char * obtener_usr_displ(char * bffr){
	/*	Esta funcion recibe un mensaje recibido por el servidor (el buffer leido). Extrae el usuario que
	 	envio el mensaje del buffer previamente leido. Esto se hace de acuerdo al formato escogido que tienen
	 	los mensajes enviados desde el servidor al los clientes
	
		VARIABLES:
			bffr 	: buffer del mensaje previamente leido
			usr 	: apuntador que contendra el nombre a retornar del emisor del mensaje
			i 		: variable auxiliar para indexar
	*/
	char *usr;
	int i=0;
	while(bffr[i]!=':'){
		i++;
	}
	usr = (char *)malloc(i+1);
	strncpy(usr,bffr,i);
	*(usr+i)= '\0';
	return usr;
}

void end(int fdr,int fdw,char * pipe1, char *pipe2){
	/* 	Funcion que recibe los pipes de escribtura y lectura del servidor asi como sus descriptores 
		de archivo para cerrarlos y hacer los unlinks correspondientes. 
		VARIABLES:
			fdr 	: descriptor del pipe de lectura
			fdw 	: descriptor del pipe de escritra
			pipe1 	: path del pipe de escritura
			pipe2 	: path del pipe de lectura
	*/
	if(close(fdw)<0){
		fprintf(stderr, "Error al cerrar pipe de escritura\n");
	}
    if(close(fdr)<0){
    	fprintf(stderr, "Error al cerrar pipe de lectura\n");
    }
    if(unlink(pipe1)<0){
    	fprintf(stderr, "Error al eliminar pipe %s\n",pipe1);
    }
    if(unlink(pipe2)<0){
    	fprintf(stderr, "Error al eliminar pipe %s\n",pipe2);
    }
}
void salirbien(int signum){
	/*	Funcion manejadora de senhales. En particular el cliente maneja la senhal SIGINT e ignora SIGPIPE
		
		VARIABLE:
			signum 	: senhal a manejar
	*/
		write(4,"-salir",TAM);
		close(3);
		close(4);
		endwin();
		printf("Conexion Terminada\n");
		exit(0);		
}
void enfocarVentana2(WINDOW *ventana2) {
    int y, x;
    getyx(ventana2, y, x);
    wmove(ventana2, y, x);
    wrefresh(ventana2);
}

/* Borra el contenido de la ventana 2 y ubica el cursor en la esquina 
 * superior izquierda de esta ventana.
 */
void limpiarVentana2(WINDOW *ventana2) {
    wclear(ventana2);
    mvwhline(ventana2, 0, 0, 0, 20); // Dibujar la línea horizontal
    wmove(ventana2, 1, 0);
    wrefresh(ventana2);
}