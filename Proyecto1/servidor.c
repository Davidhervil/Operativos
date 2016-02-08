#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define TAM 2048
int main(int argc, char *argv[]){ 
	char * pipe_com;
	char * com_buff[TAM];
	char * usuarios[20]={NULL};
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
		nam_given_size=strlen(argv[1]);
		pipe_com = (char *)malloc(tmp_part+nam_given_size+1);
		memcpy(pipe_com,"/tmp/",tmp_part);
		memcpy(pipe_com + tmp_part,argv[1],nam_given_size+1);

	}else{
		fprintf(stderr, "Uso esperado: %s [pipe]\n", argv[0]);
		return 1;
	}

	mkfifo(pipe_com,0777);
	com_fd = open(pipe_com,O_RDONLY | O_NONBLOCK);
	FD_SET(com_fd,&comm);
	while(1){
		//SELECT
		//Chequear pipe de comunicacion
		comm_cpy = comm;
		comm_success = select(2,&comm_cpy,NULL,NULL, &tv);
		if(comm_success == -1){
			perror("Communication Error");
		}else if(comm_success){
			read(com_fd,com_buff,TAM);
			//obtener_usuario(com_buff,)
			//obtener_pipes(com_buff,&pipe_r,&pipe_w)
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
