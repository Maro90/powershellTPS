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

#define PUERTO 10016
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

//---------------------------------------------------------------------------------------------------

int mostrarPregunta(t_comunicacion * comunicacion);
void iniciarJuego();

//---------------------------------------------------------------------------------------------------

int main(int argc, char *argv []) {

	if( argc < 1){
		printf("Error, debe pasar la IP del servidor por parámetro.");
		return -1;
	}

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
	gets(respuesta.texto);
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
		printf("Pregunta %d\n seguir %d\n",nroPregunta, pregunta.seguir);
	    printf("%s\n",pregunta.pregunta);

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
	    // while (respuesta < 1 || respuesta > 4) { /* Se verifica la respuesta sea válida */
		//     printf("Debe responder una de las opciones\n");
		//     printf("Ingrese la respuesta: ");
	    //     scanf("%d", &respuesta);
	    // }
		return respuesta;
}
