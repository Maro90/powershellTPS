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

#define PUERTO 10019


typedef struct{
    int seguir;
    char pregunta[100];
    char respuestas[4][100];
} t_pregunta_cliente;


int main(int argc, char *argv []) {
	char Cadena[100];

	struct sockaddr_in Direccion;
	struct servent *Puerto;
	struct hostent *Host;
	int Socket_Con_Servidor;

	Host = gethostbyname ("localhost");
	if (Host == NULL)
		return -1;

	Direccion.sin_family = AF_INET;
	// Direccion.sin_addr.s_addr = ((struct in_addr *)(Host->h_addr))->s_addr;
	Direccion.sin_port = htons(PUERTO);
	Direccion.sin_addr.s_addr = inet_addr("127.0.0.1");

	
	Socket_Con_Servidor = socket (AF_INET, SOCK_STREAM, 0);
	if (Socket_Con_Servidor == -1)
		return -1;

		printf("socket\n");

	if (connect (Socket_Con_Servidor, (struct sockaddr *)&Direccion, sizeof (Direccion)) == -1){
		printf ("No puedo establecer conexion con el servidor\n");
		exit (-1);
	}
		printf("conect\n");

	strcpy (Cadena, "Hola");
	Escribe_Socket (Socket_Con_Servidor, Cadena, 5);


	t_pregunta_cliente pregunta;
(
	Lee_Socket (Socket_Con_Servidor, &pregunta, sizeof(t_pregunta_cliente)));
	printf ("Soy cliente, He recibido : %s\n", pregunta.pregunta);

	close (Socket_Con_Servidor);
}