#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <sys/file.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include "Socket.h"
#include <arpa/inet.h>

/*#################################################
#			  Sistemas Operativos			 	#
#		Trabajo Práctico 3 - Ejericio 4	    	#
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


#define SERVICIO_FIN_JUEGO 0
#define SERVICIO_NOMBRE_JUGADOR 1
#define SERVICIO_PREGUNTA 2
#define SERVICIO_TIEMPO 3
#define SERVICIO_FIN_PREGUNTAS 4

//---------------------------------------------------------------------------------------------------

typedef struct{
    int servicio;
    int rta;
    char texto[30];
} t_respuesta_cliente;

typedef struct{
    int servicio;
    char pregunta[100];
    char respuestas[4][100];
} t_comunicacion;

typedef struct{
    int ultimo;
    int puntaje;
    char nombre[100];
} t_comunicacion_resultados;


//---------------------------------------------------------------------------------------------------

int Socket_Con_Servidor;
int PUERTO = 10016;
char ip[30];
int respondio = 0;

pthread_t   threadResponder;
//---------------------------------------------------------------------------------------------------

int mostrarPregunta(t_comunicacion * comunicacion);
void iniciarJuego();
void * responder(t_comunicacion * pregunta);
void mostrarResultados();
//---------------------------------------------------------------------------------------------------

int main(int argc, char *argv []) {



    if (argc < 2){
        printf("Error en la llamada, utilice -h para recibir más información.\n");
		exit(EXIT_FAILURE);
    }

	if (argc == 2 && (strcmp(argv[1], "-h") == 0  || strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-help") == 0) ){
		//hacer cosas de get-help
		printf("Modo de empleo: ./Cliente ip puerto \n");
		printf("ejemplo de ejecucion localmente: ./Cliente 127.0.0.1 10005 \n");
		printf( " \n");
		printf( "Cliente levanta el juego para conectarse a preguntanos ");
		printf( "\n");
		printf( " debe recibir como parametros: \n");
		printf( "	    -obligatorio: IP que se utilizará para la comunicación con el server.\n");
		printf( "	    -obligatorio: PUERTO que se utilizará para la comunicación con el server.\n");
		printf( "	    -opcional: h, -help muestra esta ayuda y finaliza \n");
		printf( "\n" );
		exit(0);
	}

	if( argc < 2){
		printf("Error, debe pasar la IP y el PUERTO del servidor por parámetro\n");
		printf("Error en la llamada, utilice -h para recibir más información.\n");
		return -1;
	}

	strcpy(ip,argv[1]);			//IP
    PUERTO = atoi(argv[2]);  //Puerto

	char Cadena[100];

	struct sockaddr_in Direccion;
	struct servent *Puerto;
	struct hostent *Host;

	printf("\tBienvenido a Preguntanos\n");
	printf("Conectando con el server...\n");

	Host = gethostbyname ("localhost");
	if (Host == NULL)
		return -1;

	Direccion.sin_family = AF_INET;
	// Direccion.sin_addr.s_addr = ((struct in_addr *)(Host->h_addr))->s_addr;
	Direccion.sin_port = htons(PUERTO);
	Direccion.sin_addr.s_addr = inet_addr(argv[1]);

	
	Socket_Con_Servidor = socket (AF_INET, SOCK_STREAM, 0);
	if (Socket_Con_Servidor == -1)
		return -1;

	if (connect (Socket_Con_Servidor, (struct sockaddr *)&Direccion, sizeof (Direccion)) == -1){
		printf ("No puedo establecer conexion con el servidor\n");
		exit (-1);
	}

	printf("Conectado con el server\n");

	t_respuesta_cliente respuesta;

    printf("Ingrese su nombre: ");
	fflush( stdin );
	fgets(respuesta.texto,100,stdin);

	int i = strlen(respuesta.texto)-1;
  	if( respuesta.texto[ i ] == '\n') 
      	respuesta.texto[i] = '\0';

	fflush( stdin );
	respuesta.servicio = SERVICIO_NOMBRE_JUGADOR;
	Escribe_Socket (Socket_Con_Servidor, &respuesta, sizeof(t_respuesta_cliente));

	printf("Espere, en breve iniciarán las preguntas\n");

	iniciarJuego();

}
//---------------------------------------------------------------------------------------------------

void iniciarJuego(){
	int nroPregunta = 0;
	int jugando = 1;
	t_comunicacion comunicacion;

	while(jugando == 1){
		Lee_Socket(Socket_Con_Servidor, &comunicacion, sizeof(t_comunicacion));
		
		switch(comunicacion.servicio){
			case SERVICIO_PREGUNTA:	
				nroPregunta++;
				respondio = 0;
    			pthread_create(&threadResponder, NULL, (void*)responder, (void*)&comunicacion);

				break;
			
			case SERVICIO_FIN_JUEGO:
				jugando = 0;

				break;

			case SERVICIO_TIEMPO:
				pthread_cancel(threadResponder);
				if(respondio == 0){
					printf("\n---------------------------------------------------------------------\n");
					printf("\n\tTiempo agotado\n");
					printf("Esperando siguiente pregunta\n");
				}

				break;
			
			case SERVICIO_FIN_PREGUNTAS:
				//Dejo de leer preguntas, espero listado de jugadores
				system("clear");
				mostrarResultados();
				jugando = 0;

		}
	}
	printf("Finalizo el juego.\n");
	printf("Salio y cierra socket\n\n");

	close (Socket_Con_Servidor);

}

//---------------------------------------------------------------------------------------------------

void * responder(t_comunicacion * pregunta){
		t_respuesta_cliente resp;
		system("clear");
		int respuesta = mostrarPregunta(pregunta);
		system("clear");
		printf("Esperando siguiente pregunta\n");
		respondio = 1;
		if (respuesta !=-1){
			resp.servicio = SERVICIO_PREGUNTA;
			resp.rta = respuesta;
			Escribe_Socket (Socket_Con_Servidor, &resp, sizeof(t_respuesta_cliente));
		}

	return NULL;
}


int mostrarPregunta(t_comunicacion * comunicacion){
		if(comunicacion->servicio == SERVICIO_FIN_JUEGO){
			return -1;
		}
        int respuesta = 0;
	    printf("\n---------------------------------------------------------------------\n");
	    printf("%s\n",comunicacion->pregunta);
        for(int i=0; i<4; i++){
            printf("RTA %d: %s\n",i+1,comunicacion->respuestas[i]);
        }
        printf("Ingrese la respuesta: ");
	    scanf("%d", &respuesta);
		fflush( stdin );
	    // while (respuesta < 1 || respuesta > 4) { /* Se verifica la respuesta sea válida */
		//     printf("Debe responder una de las opciones\n");
		//     printf("Ingrese la respuesta: ");
	    //     scanf("%d", &respuesta);
	    // }
		return respuesta;
}

void mostrarResultados(){
	int hayMas = 1;
	
	printf("*******************************************************\n");
    printf("Resultados:\n\n");

	while(hayMas == 1){
		t_comunicacion_resultados comunicacion;
		Lee_Socket(Socket_Con_Servidor, &comunicacion, sizeof(t_comunicacion_resultados));
		
		printf("Nombre: %s\tPuntaje: %d\n", comunicacion.nombre, comunicacion.puntaje);

		if (comunicacion.ultimo == 1){ //Cuando mande el ultimo dejo de esperar resultados.
			hayMas = 0;
		}
	}
	printf("\n*******************************************************\n");

}