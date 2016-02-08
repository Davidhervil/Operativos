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

char* crearPipe_w(char* usuario){
	int fd_w;
	char* dir_w = "/tmp/w_";
    char* escritura = concat(dir_w,usuario);
    mkfifo(escritura, 0666);
    fd_w = open(escritura, O_WRONLY);
    return escritura;
}

char* crearPipe_r(char* usuario){
	int fd_r;
	char* dir_l = "/tmp/r_";
	char* lectura = concat(dir_l,usuario);
    mkfifo(lectura, 0666);
    fd_r = open(lectura,O_RDONLY);
    return lectura;
}
int conectarServidor(char* pipe_r,char* pipe_w,char* pipe_serv){
	int fd;
	char* pasar;
	char* concatenar;
	concatenar = concat(pipe_r,"::");
	pasar = concat(concatenar,pipe_w);
	fd = open(pipe_serv, O_WRONLY |O_NONBLOCK);
	write(fd, pasar, sizeof(pasar));
    	close(fd);
    	return(0);
}

int main(int argc, char *argv[]){
	size_t tmp_part=strlen("/tmp/");
	size_t nam_given_size;
	size_t dflt_usr_len=strlen(getenv("USER"));
	char * usuario;
	char * pipe_com;

	if(argc==1){
		//Acciones por defecto
		usuario=(char *)malloc(dflt_usr_len+1);
		usuario=getenv("USER");
		pipe_com="/tmp/servidor";

	}else{
		if(argc==2){
			if(argv[1][0]=='-'){
				//Mensaje de error
				fprintf(stderr, "Uso esperado: %s [-p pipe] [usuario]\n", argv[0]);
				return 1;

			}else{
				//Asignamos al usuario el nombre proporcionado
				usuario = (char *)malloc(strlen(argv[1])+1);
				memcpy(usuario,argv[1],strlen(argv[1]));

				//Pipe predeterminado
				pipe_com="/tmp/servidor";
			}
		}else{
			if(argc==3)
				if(strcmp(argv[1],"-p")==0){
					//Colocamos el nombre del pipe de comunicacion dado por el usuario
					nam_given_size = strlen(argv[2]);
					pipe_com = (char *)malloc(tmp_part+nam_given_size+1);
					memcpy(pipe_com,"/tmp/",tmp_part);
					memcpy(pipe_com + tmp_part,argv[2],nam_given_size+1);

					//Usuario predeterminado
					usuario=(char *)malloc(dflt_usr_len+1);
					usuario=getenv("USER");

				}else{
					//Mensaje de error
					fprintf(stderr, "Uso esperado: %s [-p pipe] [usuario]\n", argv[0]);
					return 1;
				}
			else{
				if(argc!=4){
					//Mensaje de error
					fprintf(stderr, "Uso esperado: %s [-p pipe] [usuario]\n", argv[0]);
					return 1;
				}else{
					if(strcmp(argv[1],"-p")==0){
						//Colocamos el nombre del pipe de comunicacion dado por el usuario
						nam_given_size = strlen(argv[2]);
						pipe_com = (char *)malloc(tmp_part+nam_given_size+1);
						memcpy(pipe_com,"/tmp/",tmp_part);
						memcpy(pipe_com + tmp_part,argv[2],nam_given_size+1);
						
						//Asignamos al usuario el nombre proporcionado
						usuario = (char *)malloc(strlen(argv[3])+1);
						memcpy(usuario,argv[1],strlen(argv[3]));
					}else{
						//Mensaje de error
						fprintf(stderr, "Uso esperado: %s [-p pipe] [usuario]\n", argv[0]);
						return 1;	
					}
				}
			}
		}
	}
	conectarServidor(crearPipe_r(usuario),crearPipe_w(usuario),pipe_com);
	/*int fd;
	char * myfifo="/tmp/myfifio";
	char buf[MAX_BUF];
	fd = open(myfifo, O_RDONLY);
	read(fd, buf,MAX_BUF);
	printf("Recieved: %s\n",buf );
	close(fd);
	return 0;*/
}
