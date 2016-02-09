#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define TAM_BUFFER 2048
#define MAX_USR 20

void obtener_usuario(char * buffer, char * usuario){
	int total,i=0;
	while(buffer[i]!='\0'){
		i++;
	}
	usuario=(char *)malloc(i+1);
	i=0;
	while(buffer[i]!='\0'){
		usuario[i]=buffer[i];
	}
	usuario[i]='\0';
}

void obtener_pipe_r(char * usr, char * piper){
}

int anhadir_usuario(char * conjunto[], char * usr){
	int i=0;
	while(conjunto[i]!=NULL){
		i++;
	}
	if(i == MAX_USR){
		return 0;
	}else{
		conjunto[i] = usr;
		return 1;
	}
}
int main(int argc, char *argv[]){ 
	char * pipe_com;
	char * usuarios[MAX_USR]={NULL};
	char * usuario_aux;
	char * pipe_r;
	char * pipe_w;
	char com_buff[TAM_BUFFER];

	int fd_lectura[20];
	int fd_escritura[20];
	int com_fd,comm_success;
	size_t tmp_part=strlen("/tmp/");
	size_t nam_given_size;

	fd_set readfds,writefds,comm,comm_cpy;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	FD_ZERO(&comm);
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

	if(argc==1){
		pipe_com = "/tmp/servidor1210761-1210796";

	}else if(argc==2){
		nam_given_size = strlen(argv[1]);
		pipe_com = (char *)malloc(tmp_part+nam_given_size+1);
		memcpy(pipe_com,"/tmp/",tmp_part);
		memcpy(pipe_com + tmp_part,argv[1],nam_given_size+1);
		pipe_com[tmp_part+nam_given_size] = '\0';

	}else{
		fprintf(stderr, "Uso esperado: %s [pipe]\n", argv[0]);
		return 1;
	}

	if(mkfifo(pipe_com,0666)<0){
		fprintf(stderr,"NO SE PUDO CREAR pipe_com\n");
		return 1;
	}else{
		printf("Pipe de conexion creado\n");
	}
	if((com_fd = open(pipe_com,O_RDWR | O_NONBLOCK))<0){
		fprintf(stderr, "Error al abrir pipe de comunicacion\n");
		return 1;
	}
	FD_SET(com_fd,&comm);
	printf("Se abrio el pipe: %s y su descriptor es %d\n",pipe_com,com_fd);

	while(1){
		//printf("while true\n");
		//SELECT
		//Chequear pipe de comunicacion
		//printf("Recibido: %s\n",com_buff);
		comm_cpy = comm;
		comm_success = select(2,&comm_cpy,NULL,NULL, &tv);
		if(comm_success == -1){
			perror("Error de comunicacion");
		}else if(comm_success){
			read(com_fd,com_buff,TAM_BUFFER);
			printf("Recibido: %s\n",com_buff);
			/*
			obtener_usuario(com_buff,usuario_aux);
			obtener_pipe_r(usuario_aux,pipe_r);
			obtener_pipe_w(usuario,pipe_w);
			if(!anhadir_usuario(usuarios,usuario)){
				//enviar al usuario un mensaje de no poder agregar
			*/
		}
	}


	/*int fd;
	char * myfifo="/tmp/myfifio";

	mkfifo(myfifo, 0666);
	fd = open(myfifo, O_WRONLY);
	write(fd, "HI",sizeof("HI"));
	close(fd);
	unlink(myfifo);*/
	return 0;
}
