/*#################################################
#			  Sistemas Operativos			 	#
#		Trabajo Práctico 3 - Ejericio 3	    	#
#		Nombre del Script: Cliente.c		    #
#												#
#				Integrantes:					#
#		Arana, Juan Pablo 		33904497		#
#		Gonzalez, Mauro Daniel 	35368160		#
#		Sapaya, Nicolás Martín 	38319489		#
#												#
#		Instancia de Entrega: 1er Re Entrega	#
#												#
#################################################*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h> 
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>
#include <errno.h>

char fecha[64];
int fifo;  //FIFO
FILE* fpOut;	


int main(int argc, char *argv []) {

	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);


    if (argc == 1){
        printf("Error en la llamada, utilice -h para recibir más información.\n");
		exit(EXIT_FAILURE);
    }

	if (argc == 2 && (strcmp(argv[1], "-h") == 0  || strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-help") == 0) ){
		//hacer cosas de get-help
		printf("Modo de empleo: ./Ejercicio3 pathDelFifo tamañoDeLaEtiqueta directorioDestino \n");
		printf("ejemplo de ejecucion: ./ejercicio3 /tmp/myfifo log.log \n");
		printf( " \n");
		printf( "ejercico3 escribe clasifica los registros que le llegan por la estructura FIFO ");
		printf( "validando por el numero de caracteres recibidos de etiqueta y asignandolos aun archivo con su etiqueta y la fecha.\n");
		printf( "ejercicio3 funciona como demonio \n");
		printf( "\n");
		printf( " debe recibir como parametros: \n");
		printf( "	    -obligatorio: path del FIFO a travez del cual leera los mensajes. Si no existe se creara\n");
		printf( "	    -obligatorio: numero de caracteres que se consideraran para la etiqueta\n");
		printf( "	    -obligatorio: path del directorio donde se guardarán los archivos.\n");
		printf( "	    -opcional: h, -help muestra esta ayuda y finaliza \n");
		printf( "\n" );
		exit(0);
	}

	if( argc != 4){
        printf("Error en la llamada, utilice -h para recibir más información.\n");
		exit(EXIT_FAILURE);
	}

	int lenEtiqueta = atoi(argv[2]);  	//tamaño de la etiqueta

	DIR *dirp;
 	if( (dirp= opendir( argv[3] ) )==NULL ) {
  		printf("\nNo se pudo abrir \"%s\"\n", argv[3] );
  		printf("Verifique que sea un directorio válido.\n");
  		exit(1);
 	}
	closedir(dirp);

	//** creo el demonio
	pid_t process_id = 0;
		
	// Create child process
	process_id = fork();

	// fallo el fork()
	if (process_id < 0){
		printf("falla forrk");
		exit(EXIT_FAILURE);
	} else if (process_id > 0) {
        // Si es el padre, lo mata;
		printf("\nID del proceso: %d \n", process_id);
		exit(EXIT_SUCCESS);
	}
	
	unlink(argv[1]);
	int result = mkfifo(argv[1], 0666);
	
	if (result == -1){
		printf("Error MKFIFO\n");
		exit(EXIT_FAILURE);
	}
	// Abre el FIFO como solo lectura

	char * prefix =  malloc(lenEtiqueta);

    while (1) {

		fifo = open(argv[1], O_RDONLY);
    	if (!fifo){
			printf("Error MKFIFO\n");
			exit(EXIT_FAILURE);
		}

		char msj[1024];
		int buffer = read(fifo, msj, sizeof(msj));
       	if ( buffer > 0) {
			
			msj[buffer] = '\0';
			// consigue la fecha y la hora
			time_t t = time(NULL);
			struct tm *tm = localtime(&t);  
			strftime(fecha, sizeof(fecha), "%Y%m%d", tm);  // la formatea YYYYMMDD
			strncpy(prefix, msj, lenEtiqueta);
			char fileOutput[150];

			int pathLenght = strlen(argv[3]) - 1;
  			if( argv[3][ pathLenght ] == '/') {
				sprintf(fileOutput,"%s%s%s.txt",argv[3], prefix, fecha);	
			} else {
        		sprintf(fileOutput,"%s/%s%s.txt",argv[3], prefix, fecha);
			}

	 		fpOut = fopen( fileOutput, "a");
		    if( fpOut == NULL){
				printf("ERROR, no se pudo abrir/crear el archivo %s\n", argv[3]);
		        exit(EXIT_FAILURE);
			}
			fputs( msj, fpOut);	
			fclose(fpOut);
		}

		close(fifo);

    }
	exit(EXIT_SUCCESS);
}