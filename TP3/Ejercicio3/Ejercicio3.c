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
#		Instancia de Entrega: Entrega			#
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

char c;
char msj[128];
char fecha[64];
char linea[128 + 64];
int num;
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


    char * myfifo = argv[1];            // path del FIFO
    int lenEtiqueta = ((int)*argv[2]);  //tamaño de la etiqueta
	char * outDir = argv[3];            // path del directorio destino.

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
		exit(EXIT_SUCCESS);
	}
						
	// ignora las señales
	printf("ID del proceso: %d \n", getpid());

    while (1) {
		// Crea el FIFO:  mkfifo(<pathname>, <permission>)
		mkfifo(myfifo, 0666);
        	
		// Abre el FIFO como solo lectura
        fifo = open(myfifo, O_RDONLY);

       	if ( read(fifo, msj, sizeof(msj))<=0) {
        	// perror("No se pudo leer en el FIFO\n"); 
	 	} else {
			// consigue la fecha y la hora
			time_t t = time(NULL);
			struct tm *tm = localtime(&t);  
			strftime(fecha, sizeof(fecha), "%Y%m%d", tm);  // la formatea YYYYMMDD
		
	 		fpOut = fopen( outDir, "a");
		    if( fpOut == NULL){
				// printf("ERROR, no se pudo abrir el directorio %s\n", outDir);
		        exit(EXIT_FAILURE);
			}
	 		
        	sprintf(linea, "%s %s\n", fecha, msj);
			fputs( linea, fpOut);	

			fclose(fpOut);
		}
        close(fifo);
		unlink(myfifo);
    }

	exit(EXIT_SUCCESS);
}