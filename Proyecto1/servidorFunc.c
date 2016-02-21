#include "servidorFunc.h"

void inicializar(usuario U[]){
	/*	Esta funcion se encarga de inicializar el arreglo de estructuras usuario
	*/
	int i=0;
	for(;i<MAX_USR;i++){
		U[i].fd_lectura = -1;
		U[i].fd_escritura = -1;
		sprintf(U[i].estado,"-?");
		sprintf(U[i].nombre,"-?");
		sprintf(U[i].nombre_destino,"-?");
	}
}
char * obtener_usuario(char * buffer){
	/*	Esta funcion guarda en la variable usuario el nombre del usuario recibido en el buffer
		por el pipe de comunicacion y luego es retornada
	*/
	char * usuario;
	usuario = (char*)malloc(strlen(buffer)+1);
	memcpy(usuario,buffer,strlen(buffer)+1);
	usuario[strlen(usuario)]='\0';
	return usuario;
}

char * obtener_pipe_lect(char * usr){
	/*	Esta funcion devuelve un arreglo de caracteres con el pipe de lectura para el servidor
		dado un nombre de usuario.
		Recordar que el pipe de lectura del servidor es el de escritura del cliente. Es por
		eso que tmp_r es "/tmp/w_"

		VARIABLES:
			usr 	: Nombre de usuario
			piper 	: variable a retornar con el pipe de lesctura entre el cliente asociado y el servidor
			tmp_r 	: variable auxiliar que contiene parte del path del pipe de lectura 
			tmp_r_part : variable auxiliar que tiene el tamanho de tmp_r
	*/
	char * piper;
	char * tmp_r = "/tmp/w_";
	size_t tmp_r_part = strlen("/tmp/w_");
	piper = (char *)malloc(tmp_r_part+strlen(usr)+1);
	memcpy(piper,tmp_r,tmp_r_part);
	memcpy(piper + tmp_r_part,usr,strlen(usr) + 1);
	piper[tmp_r_part+strlen(usr)]='\0';
	return piper;
}

char * obtener_pipe_escr(char * usr){
	/*	Esta funcion devuelve un arreglo de caracteres con el pipe de escritura para el servidor
		dado un nombre de usuario.
		Recordar que el pipe de escritura del servidor es el de lectura del cliente. Es por
		eso que tmp_w es "/tmp/r_"

		VARIABLES:
			usr 	: Nombre de usuario
			pipew 	: variable a retornar con el pipe de escritura entre el cliente asociado y el servidor
			tmp_w 	: variable auxiliar que contiene parte del path de pipe de escritura 
			tmp_w_part : variable auxiliar que tiene el tamanho de tmp_r
	*/
	char * pipew;
	char * tmp_w = "/tmp/r_";
	size_t tmp_w_part = strlen("/tmp/r_");
	pipew = (char *)malloc(tmp_w_part+strlen(usr)+1);
	memcpy(pipew,tmp_w,tmp_w_part);
	memcpy(pipew + tmp_w_part,usr,strlen(usr) + 1);
	pipew[tmp_w_part+strlen(usr)]='\0';
	return pipew;
}

int esta(usuario U[], char * usr){
	/*	Esta funcion retorna 1 si el uusuario usr esta entre los usuarios conectados. De lo
		contrario retorna 0

		VARIABLES:
			usr 	: nombre del usuario
	*/
	int i=0;
	for(;i<MAX_USR;i++){
		if(strcmp(U[i].nombre,usr) == 0){
			return 1;
		}
	}
	return 0;
}

int anhadir_usuario(usuario conjunto[], char * usr, int fdr, int fdw){
	/*	Esta funcion anhade a un usuario en el arreglo de usuarios y anhade los descriptores de
		sus pipes asociaos a los arreglos fdsr y fdsw. La posicion del usuario
		corresponde con las posiciones de sus descriptores asociados.

		VARIABLES:
			conjunto	: conjunto de usuarios
			usr 		: nombre de usuario
			fdr 		: descriptor del pipe de lectura del servidor con el usuario
			fdw 		: descriptor del pipe de escritura del servidor con el usuario
	*/
	int i=0;
	while(conjunto[i].fd_lectura!=-1){
		i++;
	}
	if(i == MAX_USR){
		return 0;
	}else{
		conjunto[i].fd_lectura = fdr;
		conjunto[i].fd_escritura = fdw;
		sprintf(conjunto[i].nombre,"%s", usr);
		sprintf(conjunto[i].nombre_destino,"-?");
		return 1;
	}
}

void eliminar_usuario(usuario U[], int pos){
	/*	Esta funcion elimina el usuario en la posicion pos del arreglo de usuarios

		VARIABLES:
			U 		: Arreglo de usuarios
			pos 	: Posicion en el arreglo del usuario a eliminar
	*/
	int i=0;
	char desc_msj[TAM_BUFFER];
	if(unlink(obtener_pipe_escr(U[pos].nombre))<0){
		fprintf(stderr, "Pipe de escritura ya no existe para %s\n",U[pos].nombre);
	}
	if(unlink(obtener_pipe_lect(U[pos].nombre))<0){
		fprintf(stderr, "Pipe de lectura ya no existe para %s\n",U[pos].nombre );
	}
	for(;i<MAX_USR;i++){
		if(strcmp(U[i].nombre,"-?") != 0){
			sprintf(U[i].nombre_destino,"-?");
			sprintf(desc_msj,"Servidor:El usuario %s se ha desconectado",U[pos].nombre);
			if(write(U[i].fd_escritura,desc_msj,TAM_BUFFER)<0){
				fprintf(stderr, "Error enviando mensaje de desconexion a %s\n",U[i].nombre);
			}
		}
	}
	U[pos].fd_lectura = -1;
	U[pos].fd_escritura = -1;
	sprintf(U[pos].estado,"-?");
	sprintf(U[pos].nombre,"-?");
	sprintf(U[pos].nombre_destino,"-?");
}

int calcular_cheq(usuario conected[]){
	/* 	Esta funcion calcula el maximo descriptor entre los descriptores de lectura y escritura
		y lo retorna. DE no haber descriptores asociados (no hay usuarios) retorna -1

		VARIABLES:
			max 		: maximo descriptor
			conected 	: Arreglo de usuarios en el servidor
	*/
	int max = -1;
	int i = 0;
	for(;i<MAX_USR;i++){
		if(conected[i].fd_lectura > max){
			max = conected[i].fd_lectura;
		}
	}
	return max;
}
int procesar(char * buffer, usuario U[], int pos){
	/*	Esta funcion es la encragada de procesar todos los mensajes y comandos enviados por los clientes.
		De no concordar el mensaje con ningun comando se asumira como un mensaje hacia el ultimo usuario
		destino establecido. Si no existe este ultimo se le notificara al cliente 

		VARIABLES:
			U 			: arreglo de usuarios conectados
			pos 		: posicion del usuario que envio un mensaje al servidor
			command 	: variable que contiene el comando enviado 
			buffer 		: buffer leido por el servidor
			buffer_cpy	: copia del buffer leido
			aux_buff 	: buffer auxiliar pra escribir a los clientes
			usr 		: usuario al cual se va a escribir por el comando -escribir
			token		: variable auxiliar para extraer el comando enviado
			_void		: variable auxiliar para extraer el estado del comando -estoy
			esta 		: booleano para indicar si esta o no el usuario destino
	*/
	char command[20];
	char buffer_cpy[TAM_BUFFER],aux_buff[TAM_BUFFER];
	char usr[20];
	char * token;
	char * _void;
	int i=0,esta = 0;

	memcpy(buffer_cpy,buffer,TAM_BUFFER);
	token = strtok(buffer_cpy," ");
	sscanf(token,"%s",command);
	//Comando -escribir
	if(strcmp(command,"-escribir") == 0){
		sscanf(buffer,"-escribir %s", usr);
		//printf("usuario nurvo a escrbir %s\n", usr);
		for(;i<MAX_USR;i++){
			esta |= (strcmp(U[i].nombre,usr) == 0);
		}
		if(esta){
			if((strcmp(U[pos].nombre,usr) != 0)){
			sprintf(U[pos].nombre_destino,"%s",usr);
			}else{
				if(write(U[pos].fd_escritura,"Servidor:No es posible escribirte a ti mismo",TAM_BUFFER)<0){
					fprintf(stderr, "Error esrcibiendo a %s\n",U[pos].nombre);
				}	
			}
		}else{
			if(write(U[pos].fd_escritura,"Servidor:Usuario no encontrado",TAM_BUFFER)<0){
				fprintf(stderr, "Error esrcibiendo a %s\n",U[pos].nombre);
			}
		}
	//Comando -estoy
	}else if(strcmp(command,"-estoy") == 0){
		//printf("Se leyo comando estoy\n");
		_void = strtok(buffer," \n");
		_void = strtok(NULL,"\n");
		if(_void!=NULL){
			sprintf(buffer_cpy,"%s",_void);
			//printf("El estado leido es %s\n",buffer_cpy);
			sprintf(U[pos].estado,"%s",buffer_cpy);
		}else{
			if(write(U[pos].fd_escritura,"Servidor:Operacion invalida",TAM_BUFFER)<0){
				fprintf(stderr, "Error esrcibiendo a %s\n",U[pos].nombre);
			}
		}
		for(;i<MAX_USR;i++){
				if(strcmp(U[i].nombre_destino,U[pos].nombre) == 0){
					sprintf(buffer_cpy,"Servidor:%s cambio su estado a %s",U[pos].nombre,U[pos].estado);
					if(write(U[i].fd_escritura,buffer_cpy,TAM_BUFFER)<0){
						fprintf(stderr, "Error esrcibiendo a %s\n",U[i].nombre);
					}
				}
			}
	//Comando -quien
	}else if(strcmp(command,"-quien") == 0){
		sprintf(buffer_cpy,"Servidor: Lista de conectados\n");
		for(;i<MAX_USR;i++){
			if(strcmp(U[i].nombre,"-?") != 0){
				sprintf(aux_buff,"Nombre: %s Estado: %s\n",U[i].nombre,U[i].estado);
				strcat(buffer_cpy,aux_buff);
			}
		}
		if(write(U[pos].fd_escritura,buffer_cpy,TAM_BUFFER)<0){
			fprintf(stderr, "Error esrcibiendo a %s\n",U[pos].nombre);
		}

	}
	//Sino, es un mensaje
	else{
		if(strcmp(U[pos].nombre_destino,"-?") != 0){
			sprintf(buffer_cpy,"%s:%s",U[pos].nombre,buffer);
			for(;i<MAX_USR;i++){
				if(strcmp(U[i].nombre,U[pos].nombre_destino) == 0){
					break;
				}
			}
			if(write(U[i].fd_escritura,buffer_cpy,TAM_BUFFER)<0){
				fprintf(stderr, "Error esrcibiendo a %s\n",U[i].nombre);
			}
		}else{
			if(write(U[pos].fd_escritura,"Servidor:Usuario no asignado",TAM_BUFFER)<0){
				fprintf(stderr, "Error esrcibiendo a %s\n",U[pos].nombre);
			}
		}

	}
	return 1;
}

void explotar(int signum){
	/*	Esta funcion es el manejador de senhales. SOLO SE MANEJAN LAS SENHALES:
		SIGINT
		SIGPIPE
		en el caso de SIGPIPE esta es ignorada

			VARIABLES:
			boom 		: numero de la senhal SIGINT
			pipe_com 	: pipe decomunicaciones del servidor (variable global)
	*/
	int boom = 3;
	for(;boom<45;boom++){
		unlink(pipe_com);
		write(boom,"-salir",TAM_BUFFER);
		close(boom);
	}
	exit(0);
}
