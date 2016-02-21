#include "servidorFunc.h"

int main(int argc, char *argv[]){ 
	/*	Funcion principal del servidor. Crea su pipe de comunicacion, cierra cualquier pipe que pueda llamarse igual
		para evitar errores antes de abrirlo.

		VARIABLES:
			usuario_aux 	: variable para almacenar temporalmente el nombre de un usuario que solicita conexion	
			pipe_r 			: variable para almacenar temporalmente el nombre del pipe de lectura del servidor hacia el cliente
			pipe_w			: variable para almacenar temporalmente el nombre del pipe de escritura del servidor hacia el cliente
			com_buff 		: buffer de comunicacion
			conn_msj 		: mensaje de conexion a enviar a todos los usuarios cuando entra uno nuevo
			conectados 		: arreglo de usuarios ocnectados
			dafuq 			: numero de bytes leidos en el pipe de comunicacion
			ngga 			: numero de pipes leidos en los pipe de lectura cliente-servidor
			com_fd 			: descriptor del pipe de comunicacion
			comm_success 	: resultado de hacer select del pipe de comunicacion
			fdread_aux 		: variable auxiliar para almacenar temporalmente el descriptor de lectura de un usuario nuevo
			fdwrite_aux 	: variable auxiliar para almacenar temporalmente el descriptor de escritura de un usuario nuevo
			cheq 			: maximo descriptor en el set del select de los pipes cliente-servidor
			disp 			: resultado de hacer select de los pipes cliente-servidor
			tmp_part 		: variable auxiliar para guardar el tamanho del path /tmp/ del pipe de comunicacion
			nam_given_size 	: tamanho del nombre del pipe introducido por el usuario
			tv 				: estructura de tiempo para los select
	*/
	signal(SIGINT,explotar);
	signal(SIGPIPE,SIG_IGN);
	char * usuario_aux;
	char * pipe_r;
	char * pipe_w;
	char com_buff[TAM_BUFFER];
	char conn_msj[TAM_BUFFER];

	usuario conectados[MAX_USR];

	int dafuq,ngga;
	int com_fd,comm_success,fdread_aux,fdwrite_aux,cheq,disp;
	size_t tmp_part=strlen("/tmp/");
	size_t nam_given_size;

	fd_set readfds,comm,comm_cpy,readfds_cpy;

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	FD_ZERO(&comm);
	FD_ZERO(&readfds);
	inicializar(conectados);

	//Escogencia del nombre del pipe de comunicacion
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
	if(unlink(pipe_com)<0){//Eliminamos un pipe que pueda no haber sido borrado por un servidor anterior
		fprintf(stderr, "Error eliminando pipe residual %s\n",pipe_com);
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
		//SELECT de lectura de los clientes
		readfds_cpy = readfds;
		cheq = calcular_cheq(conectados);
		if(cheq != -1){
			disp = select(cheq+1,&readfds_cpy,NULL,NULL,&tv);
			if(disp == -1){
				perror("Error de seleccion de pipes dobles");	
			}else if(disp){
				int i = 0;
				for(; i<MAX_USR;i++){
					if(FD_ISSET(conectados[i].fd_lectura,&readfds_cpy)){
						ngga = read(conectados[i].fd_lectura,com_buff,TAM_BUFFER);
						if(ngga>0 && strlen(com_buff)!=0){
							com_buff[strlen(com_buff)]='\0';
							//Procesado
							if((strcmp(com_buff,"-salir") == 0) ){
								
								FD_CLR(conectados[i].fd_lectura,&readfds);
								close(conectados[i].fd_lectura);
								close(conectados[i].fd_escritura);
								//unlink(obtener_pipe_escr(conectados[i].nombre));
								//unlink(obtener_pipe_lect(conectados[i].nombre));
								eliminar_usuario(conectados,i);
							}
							else{
								procesar(com_buff,conectados,i);
							}
							//printf("Mensaje de %s : %s\n",conectados[i].nombre,com_buff);
						}
					}
				}
			}
		}
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
			if(dafuq!=0){
				printf("Solicitud de conexion: %s\n",com_buff);
				printf("Obteniendo usuario\n");
				usuario_aux = obtener_usuario(com_buff);
				printf("Usuario obtenido\n");
				pipe_r = obtener_pipe_lect(usuario_aux);
				pipe_w = obtener_pipe_escr(usuario_aux);
				if((fdwrite_aux = open(pipe_w,O_WRONLY | O_NONBLOCK))<0){
					fprintf(stderr, "Error al abrir pipe de escritura al usuario %s\n",usuario_aux);
					return -1;
				}
				if((fdread_aux = open(pipe_r, O_RDONLY|O_NONBLOCK))<0){
					fprintf(stderr, "Error al abrir pipe de lectura del usuario %s\n",usuario_aux);
					return -1;
				}
				printf("Pipes del usuario %s: %s desc %d %s desc %d\n",usuario_aux,pipe_w,fdwrite_aux,pipe_r,fdread_aux);
				if(!esta(conectados,usuario_aux)){
					if(!anhadir_usuario(conectados,usuario_aux,fdread_aux,fdwrite_aux)){
						write(fdwrite_aux,"Servidor lleno",TAM_BUFFER);
						close(fdwrite_aux);
						close(fdread_aux);
					}else{
						for(ngga=0;ngga<MAX_USR;ngga++){
							if(conectados[ngga].fd_escritura!=-1){
								sprintf(conn_msj,"Servidor:%s se ha conectado.",usuario_aux);
								write(conectados[ngga].fd_escritura,conn_msj,TAM_BUFFER);
							}
						}
						write(fdwrite_aux,"Servidor:Comandos disponibles\n -escribir <usuario>\n -estoy <estado>\n -salir",TAM_BUFFER);
						FD_SET(fdread_aux,&readfds);
					}
				}else{
					printf("Usuario existente\n");
					write(fdwrite_aux,"Nombre de usuario existente. Conexion Fallida",TAM_BUFFER);
					close(fdwrite_aux);
					close(fdread_aux);
				}
			}
		}
	}
	return 0;
}