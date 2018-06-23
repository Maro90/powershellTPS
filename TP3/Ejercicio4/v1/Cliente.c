#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <dirent.h> 
#include <unistd.h>
#include "Socket_Cliente.h"
#include "Socket.h"

/******************************************/
typedef struct{
    int seguir;
    char pregunta[100];
    char respuestas[4][100];
} t_comunicacion;

/***********************    VARIABLES     ************************/
int PUERTO = 10019;
int jugando = 1;
int Socket_Con_Servidor;

/***********************    PROTOTIPOS FUNCIONES     ************************/
void desconectar ();
void capturador_seniales(int );
void iniciarJuego();
void mostrarPregunta(t_comunicacion * comunicacion);
/***********************    MAIN    ***** ************************/
int main(int argc, char *argv []) {
	if(argc !=2) { /* Indica la sintaxis correcta a utilizar */
		printf("ERROR. Sintaxis: %s < ip servidor > \n", argv[0] );
		return 1;
	}
	struct sigaction sig_action; 

	/* Posibles señales a recibir */

	sig_action.sa_handler=capturador_seniales;  
	sigaction(SIGTERM, &sig_action, NULL);
	sigaction(SIGINT, &sig_action, NULL);

	/* Creo la conexion con el servidor */
	Socket_Con_Servidor = abreConexion(argv[1], PUERTO);

	if (Socket_Con_Servidor == -1) { /*Se verifica que se realizo correctamente la conexion */
		printf ("No puedo establecer conexion con el servidor\n");
		return 1;
	}
    iniciarJuego();
}


/***********************    FUNCIONES     ************************/

void iniciarJuego(){

    t_comunicacion comunicacion;

    while(jugando){
        leeSocket(Socket_Con_Servidor, &comunicacion, sizeof(comunicacion));
        printf("SIGO? %d",comunicacion.seguir);
        	    printf("%s\n",comunicacion.pregunta);

        // if ( comunicacion.seguir == 0){
            mostrarPregunta(&comunicacion);
        // } else {
            // jugando = 0;
        // }
    }
    desconectar();
}

void mostrarPregunta(t_comunicacion * comunicacion){
        int respuesta = 0;
	    printf("\n---------------------------------------------------------------------");
        printf("Pregunta");
	    printf("%s\n",comunicacion->pregunta);
        for(int i=0; i<4; i++){
            printf("RTA %d: %s\n",i+1,comunicacion->respuestas[i]);
        }
        printf("Ingrese la respuesta: ");
	    scanf("%d", &respuesta);
	    while (respuesta < 1 || respuesta > 4) { /* Se verifica la respuesta sea válida */
		    printf("Debe responder una de las opciones\n");
		    printf("Ingrese la respuesta: ");
	        scanf("%d", &respuesta);
	    }
        escribeSocket (Socket_Con_Servidor, &respuesta, sizeof(int));
}





//Finalización abrupta.
void capturador_seniales(int signum) {
	switch (signum) 
	{
	case SIGTERM:
		desconectar();
		break;

	case SIGINT:
		desconectar();
		break;
	}
}

//Llamado a desconectar
void desconectar () {
	printf("\nEl cliente finalizo!\n");
	close (Socket_Con_Servidor);
	exit(EXIT_FAILURE);
}

