#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

char* concat(char *s1, char *s2){
	char *result;
    result = (char*)malloc(strlen(s1)+strlen(s2)+1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char* crearPipe_w(char* usuario){
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

int main(int argc, char *argv[]){ 
	int fd_w;
	char* pwrite;
	char* Arturo = "Arturo";

	printf("Se ha creado %s\n",pwrite = crearPipe_w(Arturo));
	printf("Se ha creado %s\n",pwrite = crearPipe_w(Arturo));
	
	if((fd_w = open(pwrite, O_WRONLY|O_NONBLOCK))<0){
		printf("No se abrio %s\n",pwrite);
		return 1;
	}

}
