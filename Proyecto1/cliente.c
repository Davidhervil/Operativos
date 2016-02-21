#include "clienteFunc.h"

WINDOW *ventana1, *ventana2;

int main(int argc, char *argv[]){					// argc se asigna solo, es el numero de argumentos que se pasan por terminal.
	signal(SIGINT,salirbien);
	signal(SIGPIPE,SIG_IGN);
	int fd_w,fd_r,aux;									// Filedescriptors de los dos pipes que se crean.
	size_t tmp_part=strlen("/tmp/");				
	size_t nam_given_size;							// Tamanio del nombre proporcionado
	size_t dflt_usr_len=strlen(getenv("USER"));		// Tamanio del nombre de usuario por defecto
	
	char * usuario;									// usuario y pipe_com son apuntadores.
	char * usuario_dest;
	char * usuario_displ;
	char * pipe_com;
	char* dir_wr = "/tmp/w_";
    char* pwrite;
    char* pread;
    char* dir_re = "/tmp/r_";
	char com_buff[TAM];
	char buffer[TAM];
	
	int comm_success,fdread_aux,fdwrite_aux,leido,i=0,j=0,booleano=0,espacio=0;
	fd_set readfds,writefds,comm,comm_cpy,readfds_cpy,writefds_cpy;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	FD_ZERO(&comm);
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	memset(buffer,0,sizeof(buffer));

	if(argc==1){									// Si solo se proporciona un argumento, entonces el pipe y el usuario se toman por defecto
		//Acciones por defecto
		usuario=(char *)malloc(dflt_usr_len+1);		// Pedimos espacio en memoria para el nombre del usuario por defecto y el caracter nulo
		usuario=getenv("USER");						// Asignamos el nombre de usuario
		pipe_com="/tmp/servidor1210761-1210796";					// asignamos el nombre del pipe por defecto segun lo especificado.

	}else{
		if(argc==2){
			if(argv[1][0]=='-'){					// Si hay 2 argumentos y el primer argumento empieza con "-" dar error
				//Mensaje de error
				fprintf(stderr, "Uso esperado: %s [-p pipe] [usuario]\n", argv[0]);
				return 1;

			}else{									// En caso contrario, asignar el nombre del usuario.
				//Asignamos al usuario el nombre proporcionado
				usuario = (char *)malloc(strlen(argv[1])+1);
				memcpy(usuario,argv[1],strlen(argv[1]));
				usuario[strlen(usuario)] = '\0';

				//Pipe predeterminado
				pipe_com="/tmp/servidor1210761-1210796";
			}
		}else{
			if(argc==3)
				if(strcmp(argv[1],"-p")==0){
					//Colocamos el nombre del pipe de comunicacion dado por el usuario
					nam_given_size = strlen(argv[2]);
					pipe_com = (char *)malloc(tmp_part+nam_given_size+1);
					memcpy(pipe_com,"/tmp/",tmp_part);
					memcpy(pipe_com + tmp_part,argv[2],nam_given_size+1); /////////////////
					pipe_com[strlen(pipe_com)] = '\0';
					printf("%s\n",pipe_com);

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
						pipe_com[strlen(pipe_com)] = '\0';

						//Asignamos al usuario el nombre proporcionado
						usuario = (char *)malloc(strlen(argv[3])+1);
						memcpy(usuario,argv[3],strlen(argv[3]));
					}else{
						//Mensaje de error
						fprintf(stderr, "Uso esperado: %s [-p pipe] [usuario]\n", argv[0]);
						return 1;	
					}
				}
			}
		}
	}
	// Creamos los pipes de escritura y lectura
	printf("Se ha creado %s\n",pwrite = crearPipe_w(usuario));
	printf("Se ha creado %s\n",pread = crearPipe_r(usuario));

	//Abrimos el de lectura
	if((fd_r = open(pread,O_RDONLY|O_NONBLOCK))<0){
		fprintf(stderr,"No se abrio %s\n",pread);
		return 1;
	}	
	//Nos conectamos con el servidor
	if(conectarServidor(usuario,pipe_com)){
	}else{
		fprintf(stderr,"Error al conectarServidor\n");
		return -1;
	}

	//Abrimos el pipe de escritura

	if((fd_w = open(pwrite, O_WRONLY))<0){
		fprintf(stderr,"No se abrio %s\n",pwrite);
		return 1;
	}

	FD_SET(fd_r,&comm);

	//							INTERFAZ	
	/////////////////////////////////////////////////////////////////////
	    
	    initscr(); // Inicializar la biblioteca ncurses
    
    if (LINES < LINES_MIN || COLS < COLS_MIN) {
        endwin(); // Restaurar la operación del terminal a modo normal
        printf("El terminal es muy pequeño para correr este programa.\n");
        exit(0);
    }

    // Opciones de la biblioteca ncurses
    cbreak();
    nonl();

    int alto1 = LINES - ALTO; // Alto de la ventana 1
    ventana1 = newwin(alto1, 0, 0, 0); // Crear la ventana 1
    ventana2 = newwin(ALTO, 0, alto1, 0); // Crear la ventana 2
    scrollok(ventana1, TRUE); //Activar el corrimiento automático en la ventana 1
    scrollok(ventana2, TRUE);
    limpiarVentana2(ventana2); // Dibujar la línea horizontal

    nodelay(ventana2,TRUE);

    while(1) {
        //wgetnstr(ventana2, buffer, TAM); // Leer una línea de la entrada
        //aux = write(fd_w,buffer,TAM);
		//wprintw(ventana1, concat(usuario," Escribiste al pipe %d: %d letras \n"), fd_w,strlen(buffer));
        i = -1;
        i = wgetch(ventana2);
        if (i < 0){
            ;
        }else{
            if(i == 13 && !booleano){ //Se presiono la tecla ENTER
                wclear(ventana2);
                mvwhline(ventana2, 0, 0, 0, 20); // Dibujar la línea horizontal
                wmove(ventana2, 1, 0);
                for(espacio=0;espacio<j;espacio++){ 	//Ciclo que verifica si el mensaje contiene solo espacios en blanco
                	if(buffer[espacio]!=' '){	//de ser asi no se enviara mensajes en blanco
                		espacio=0;
               			break;
               		}
                }
                j = 0;
                wrefresh(ventana1);
                if(strcmp(buffer, "-salir") == 0){		//Si se escribio "-salir" entonces se envia al servidor el mensaje y se toman
		        	if(write(fd_w,buffer,TAM)<0){	//las medidas pertinentes
		        		fprintf(stderr, "Error al escribir en %s\n",pwrite);
		        	}
		        	end(fd_r,fd_w,pwrite,pread);
		        	endwin();
		            break;
        		}
                else{
                	if(strlen(buffer)!=0 && !espacio){	//Si esta en blanco o vacio el mensaje no se envia
	                	wprintw(ventana1, "%s: %s\n",usuario,buffer);
	                    wrefresh(ventana1);
	                    write(fd_w,buffer,TAM);
	                    limpiarVentana2(ventana2);
                	}
                }
                memset(buffer,0,sizeof(buffer));
            }
            else if ((i == 127) && j>0 && !booleano){	 //Borrar
                buffer[j-1] = 0;
                j--;
                wclear(ventana2);
                mvwhline(ventana2, 0, 0, 0, 20); // Dibujar la línea horizontal
                wmove(ventana2, 1, 0);
                wprintw(ventana2,"%s",buffer);
                wrefresh(ventana2);
            }else if(i == 27){			//Si se presiona alguna de las flechas
            	booleano = 2;
            }
            else{
            	if(booleano > 0){		//No mostramos los caracteres de las flechas
            		booleano--;
            		wclear(ventana2);
		            mvwhline(ventana2, 0, 0, 0, 20); // Dibujar la línea horizontal
		            wmove(ventana2, 1, 0);
		            wprintw(ventana2,"%s",buffer);
		            wrefresh(ventana2);
            	}else{				
						//Actualizamos el buffer con el caracter introducido
		            buffer[j] = (char)i;
		            j++;
		            wclear(ventana2);
		            mvwhline(ventana2, 0, 0, 0, 20); // Dibujar la línea horizontal
		            wmove(ventana2, 1, 0);
		            wprintw(ventana2,"%s",buffer);
		            wrefresh(ventana2);
            	}
            }
        }
		//Select que indica si se recibe algo del servidor
		comm_cpy = comm;
		comm_success = select(fd_r+1,&comm_cpy,NULL,NULL, &tv);
		if(comm_success == -1){
			perror("Error de comunicacion");

		}else if(comm_success){
			leido = read(fd_r,com_buff,TAM);
			if (strcmp(com_buff,"-salir") == 0){
				wprintw(ventana1,"El servidor ha cerrado. El cliente se cerrara en 10 segundos");
				wrefresh(ventana1);
				close(fd_r);
				close(fd_w);
				unlink(pwrite);
				unlink(pread);
				unlink(pipe_com);
				sleep(10);
				endwin();
				exit(0);
			}
			if(leido){
				com_buff[strlen(com_buff)]='\0';
				usuario_displ = obtener_usr_displ(com_buff);
				wprintw(ventana1, concat(usuario_displ,": %s\n"), com_buff+strlen(usuario_displ)+1);
				wrefresh(ventana1);
			}else{
				end(fd_r,fd_w,pwrite,pread);
				endwin();
				break;
			}
		}
    }

    endwin(); // Restaurar la operación del terminal a modo normal
    exit(0);
}
