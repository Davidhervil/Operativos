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
	}else if(argc==3){
		if(strcmp(argv[1],"-p")==0){
			//Colocamos el nombre del pipe de comunicacion dado por el usuario
			nam_given_size = strlen(argv[2]);
			pipe_com = (char *)malloc(tmp_part+nam_given_size+1);
			memcpy(pipe_com,"/tmp/",tmp_part);
			memcpy(pipe_com + tmp_part,argv[2],nam_given_size+1);
			//Asignamos al usuario el nombre proporcionado
			usuario = (char *)malloc(strlen(argv[3])+1);
			memcpy(usuario,argv[3],strlen(argv[3]));
		}else{
			fprintf(stderr, "Uso esperado: %s [-p pipe] [usuario]\n", argv[0]);
			return 1;
		}
	}}

	/*int fd;
	char * myfifo="/tmp/myfifio";
	char buf[MAX_BUF];
	fd = open(myfifo, O_RDONLY);
	read(fd, buf,MAX_BUF);
	printf("Recieved: %s\n",buf );
	close(fd);
	return 0;*/
}
