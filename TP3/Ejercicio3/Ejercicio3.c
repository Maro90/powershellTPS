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



int main(int argc, char *argv []) {

    if (argc == 1){
        printf("Error en la llamada, utilice -h para recibir más información.\n");
        exit(0);
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
		printf("Error, debe pasar el nombre del fifo, el tamaño de la etiqueta y el directorio de destino.\n");
		return -1;
	}

}