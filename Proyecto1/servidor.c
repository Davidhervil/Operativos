#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define TAM 2048
char* concat(char *s1, char *s2){
	char *result;
    result = (char*)malloc(strlen(s1)+strlen(s2)+1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


int main(int argc, char *argv[]){ 
	char * pipe_com;
	char com_buff[TAM];
	int fd_lectura[20],fd_escritura[20];
	int com_fd,comm_success,aux = 0;
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
		pipe_com = "/tmp/servidor1210761-1210796\n";

	}else if(argc==2){
		nam_given_size=strlen(argv[1]);
		pipe_com = (char*)malloc(tmp_part+nam_given_size+1);
		memcpy(pipe_com,"/tmp/",tmp_part);
		memcpy(pipe_com + tmp_part,argv[1],nam_given_size+1);

	}else{
		fprintf(stderr, "Uso esperado: %s [pipe]\n", argv[0]);
		return 1;
	}

	mkfifo(pipe_com,0777);
	com_fd = open(pipe_com,O_RDONLY | O_NONBLOCK);
	printf("Se abrio el pipe:\n");
	printf("%s",pipe_com);
	FD_SET(com_fd,&comm);
	while(1){
		//SELECT
		//Chequear pipe de comunicacion
		comm_cpy = comm;
		comm_success = select(2,&comm_cpy,NULL,NULL, &tv);
		aux = aux+1;
		if(comm_success == -1){
			perror("Communication Error");
		}else if(comm_success){
			printf("Recibiste algo\n");
			read(com_fd,com_buff,TAM);
			printf("%s",com_buff);
			//obtener_usuario(com_buff);
			//obtener_pipes(com_buff,pipe_r,pipe_w);
		}
	}
	return 0;
}
