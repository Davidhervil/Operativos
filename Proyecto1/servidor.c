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
	/*	Esta funcion guarda en la variable usuario el nombre del usuario recibido en el buffer
		por el pipe de comunicacion
	*/
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

void obtener_pipe_lect(char * usr, char * piper){
	/*	Esta funcion devuelve un arreglo de caracteres con el pipe de lectura para el servidor
		dado un nombre de usuario.
		Recordar que el pipe de lectura del servidor es el de escritura del cliente. Es por
		eso que tmp_r es "/tmp/w_"
	*/
	char * tmp_r = "/tmp/w_";
	size_t tmp_r_part = strlen("/tmp/w_");
	piper = (char *)malloc(tmp_r_part+strlen(usr)+1);
	memcpy(piper,tmp_r,tmp_r_part);
	memcpy(piper + tmp_r_part,usr,strlen(usr) + 1)
	piper[tmp_r_part+strlen(usr)]='\0';

}

void obtener_pipe_escr(char * usr, char * pipew){
	/*	Esta funcion devuelve un arreglo de caracteres con el pipe de escritura para el servidor
		dado un nombre de usuario.
		Recordar que el pipe de escritura del servidor es el de lectura del cliente. Es por
		eso que tmp_w es "/tmp/r_"
	*/
	char * tmp_w = "/tmp/r_";
	size_t tmp_w_part = strlen("/tmp/r_");
	pipew = (char *)malloc(tmp_w_part+strlen(usr)+1);
	memcpy(pipew,tmp_w,tmp_w_part);
	memcpy(pipew + tmp_w_part,usr,strlen(usr) + 1)
	pipew[tmp_w_part+strlen(usr)]='\0';
}

int anhadir_usuario(char * conjunto[], char * usr, int fdr, int fdw, int * fdsr, int fdsw){
	/*	Esta funcion anhade a un usuario en el arreglo de usuarios y anhade los descriptores de
		sus pipes asociaos a los arreglos fdsr y fdsw. La posicion del usuario
		corresponde con las posiciones de sus descriptores asociados.
	*/
	int i=0;
	while(conjunto[i]!=NULL){
		i++;
	}
	if(i == MAX_USR){
		return 0;
	}else{
		fdsr[i] = fdr;
		fdsr[i] = fdw;
		conjunto[i] = usr;
		return 1;
	}
}
int main(int argc, char *argv[]){ 
	char * pipe_com;
	char * usuario_aux;
	char * pipe_r;
	char * pipe_w;
	char com_buff[TAM_BUFFER];
	char * usuarios[MAX_USR]={NULL};
	char * estados[MAX_USR]={NULL};
	char * usuario_asociado[MAX_USR]={NULL};

	int dafuq;
	int fds_lectura[20]={-1};
	int fds_escritura[20]={-1};
	int com_fd,comm_success,fdread_aux,fdwrite_aux;
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
		return -1;
	}
	if(mkfifo(pipe_com,0666)<0){
		fprintf(stderr,"NO SE PUDO CREAR pipe_com\n");
		return -1;
	}else{
		printf("Pipe de conexion creado\n");
	}
	if((com_fd = open(pipe_com,O_RDWR | O_NONBLOCK))<0){
		fprintf(stderr, "Error al abrir pipe de comunicacion\n");
		return -1;
	}
	FD_SET(com_fd,&comm);
	printf("Se abrio el pipe: %s y su descriptor es %d\n",pipe_com,com_fd);
	
	while(1){
		//printf("while true\n");
		//SELECT
		//Chequear pipe de comunicacion
		//dafuq = read(com_fd,com_buff,TAM_BUFFER);
		//if(dafuq!=0)printf("Recibido: %s\n",com_buff);
		comm_cpy = comm;
		comm_success = select(com_fd+1,&comm_cpy,NULL,NULL, &tv);
		if(comm_success == -1){
			perror("Error de comunicacion");
		}else if(comm_success){
			dafuq = read(com_fd,com_buff,TAM_BUFFER);
			com_buff[dafuq]='\0';
			printf("Recibido: %s\n",com_buff);
			obtener_usuario(com_buff,usuario_aux);
			obtener_pipe_lect(usuario_aux,pipe_r);
			obtener_pipe_escr(usuario_aux,pipe_w);
			if((fdwrite_aux = open(pipe_w,O_WRONLY | O_NONBLOCK))<0){
				fprintf(stderr, "Error al abrir pipe de escritura del usuario %s\n",usuario_aux);
				return -1;
			}
			if((fdread_aux = open(pipe_r, O_RDONLY | O_NONBLOCK))<0){
				fprintf(stderr, "Error al abrir pipe de lectura del usuario %s\n",usuario_aux);
				return -1;
			}
			if(!anhadir_usuario(usuarios,usuario,fdread_aux,fdwrite_aux,fds_lectura,fds_escritura)){
				write(fdwrite_aux,"Servidor lleno",strlen("Servidor lleno")+1);
				close(fdwrite_aux);
				close(fdread_aux);
			}
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
