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
		usuario=malloc(dflt_usr_len+1);
		usuario=getenv("USER");
		pipe_com="/tmp/servidor";

	}else if(argc==4){
		if(argv[1][0]=='-'){
			if(argv[1][1]=='p'){
				nam_given_size=strlen(argv[2]);
				pipe_com = malloc(tmp_part+nam_given_size+1);
				memcpy(pipe_com,"/tmp/",tmp_part);
				memcpy(pipe_com + tmp_part,argv[2],nam_given_size+1);
			}
		}
	}

	/*int fd;
	char * myfifo="/tmp/myfifio";
	char buf[MAX_BUF];
	fd = open(myfifo, O_RDONLY);
	read(fd, buf,MAX_BUF);
	printf("Recieved: %s\n",buf );
	close(fd);
	return 0;*/
}
