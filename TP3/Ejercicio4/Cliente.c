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

//---------------------------------------------------------------------------------------------------

typedef struct{
    int servicio;
    int rta;
    char texto[30];
} t_respuesta_cliente;

typedef struct{
    int seguir;
    char pregunta[100];
    char respuestas[4][100];
} t_comunicacion;

//---------------------------------------------------------------------------------------------------

int Socket_Con_Servidor;
int PUERTO = 10016;
char ip[30];
//---------------------------------------------------------------------------------------------------

int mostrarPregunta(t_comunicacion * comunicacion);
void iniciarJuego();

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
	fflush( stdin );
	respuesta.servicio = 1;
	Escribe_Socket (Socket_Con_Servidor, &respuesta, sizeof(t_respuesta_cliente));

	printf("Espere, en breve iniciarán las preguntas\n");

	iniciarJuego();

}
//---------------------------------------------------------------------------------------------------

void iniciarJuego(){
	int nroPregunta = 0;
	int jugando = 1;
	t_comunicacion pregunta;
	t_respuesta_cliente resp;

	while(jugando == 1){
		Lee_Socket(Socket_Con_Servidor, &pregunta, sizeof(t_comunicacion));
		nroPregunta++;
		int respuesta = mostrarPregunta(&pregunta);
		if (respuesta !=-1){
			resp.servicio = 2;
			resp.rta = respuesta;
			Escribe_Socket (Socket_Con_Servidor, &resp, sizeof(t_respuesta_cliente));
		}

		if(pregunta.seguir == 0){
			jugando = 0;
		}
	}
	printf("Salio y cierra socket\n");

	close (Socket_Con_Servidor);

}

//---------------------------------------------------------------------------------------------------

int mostrarPregunta(t_comunicacion * comunicacion){
		if(comunicacion->seguir == 0){
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
