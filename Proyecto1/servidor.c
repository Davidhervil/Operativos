#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]){ 
	char * pipe_com;
	size_t tmp_part=strlen("/tmp/");
	size_t nam_given_size;

	if(argc==1){
		pipe_com = "/tmp/servidor1210761-1210796";

	}else if(argc==2){
		nam_given_size=strlen(argv[1]);
		pipe_com = malloc(tmp_part+nam_given_size+1);
		memcpy(pipe_com,"/tmp/",tmp_part);
		memcpy(pipe_com + tmp_part,argv[1],nam_given_size+1);

	}else{
		fprintf(stderr, "Uso esperado: %s [pipe]\n", argv[0]);
		return 1;
	}

	mkfifo(pipe_com, O_NOMBLOCK);

	/*int fd;
	char * myfifo="/tmp/myfifio";

	mkfifo(myfifo, 0666);
	fd = open(myfifo, O_WRONLY);
	write(fd, "HI",sizeof("HI"));
	close(fd);
	unlink(myfifo);*/
	return 0;
}
