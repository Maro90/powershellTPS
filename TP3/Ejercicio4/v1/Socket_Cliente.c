/*
* Includes del sistema
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
* Conecta con un servidor Unix, en la misma maquina.
*	Devuelve descriptor de socket si todo es correcto, -1 si hay error.
*/
int abreConexion(char *ip, int puerto)
{
	struct sockaddr_in direccion;
	int descriptor;

	direccion.sin_family = AF_INET;
	direccion.sin_port = htons(puerto);
	direccion.sin_addr.s_addr = inet_addr(ip);
    	bzero (&(direccion.sin_zero), 8);

	descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (descriptor == -1){
		return -1;			
	}
	/*
	* Devuelve 0 si todo va bien, -1 en caso de error
	*/
	if (connect(descriptor,(struct sockaddr *)&direccion,sizeof(struct sockaddr)) == -1){
		return -1;
	}
	else
	{
		return descriptor;
	}
}
