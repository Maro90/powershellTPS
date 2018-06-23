//
//
//  Funciones para la apertura de un socket servidor y la conexion con sus
//  clientes
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/*
*	Abre socket servidor UNIX. Se le pasa la ip y el puerto que desea tener
* Deja el socket preparado para aceptar conexiones de clientes.
* Devuelve el descritor del socket servidor, que se debera pasar
* a la funcion aceptaConexionCliente(). Devuelve -1 en caso de error
*/
int abreSocket(char *ip, int puerto, int nroConexiones)
{
	struct sockaddr_in direccion;
	int Descriptor;

	/*
	* Se abre el socket
	*/
	Descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (Descriptor == -1)
	 	return -1;

	/*
	* Se rellenan en la estructura direccion los datos necesarios para
	* poder llamar a la funcion bind()
	*/
	direccion.sin_family = AF_INET;

    if (!ip || *ip=='\0') {  //si no le paso ip, le pido que ponga una por default
	 direccion.sin_addr.s_addr=htonl(INADDR_ANY);
    }

    if (puerto <= 0) {  //si no le paso puerto, eligira uno aleatoriamente
        direccion.sin_port = htons(3500);
    }
    else
        direccion.sin_port = htons(puerto);
	bzero(&(direccion.sin_zero), 8);

	if (bind(Descriptor,(struct sockaddr *)&direccion,sizeof(struct sockaddr)) == -1){
		/*
		* En caso de error cerramos el socket y devolvemos error
		*/
		close(Descriptor);
		printf("Error bind\n");
		return -1;
	}
	/*
	* Avisamos al sistema que comience a atender peticiones de clientes.
	*/
	if (listen(Descriptor, nroConexiones) == -1)
	{
		close (Descriptor);
		return -1;
	}
	/*
	* Se devuelve el descriptor del socket servidor
	*/
	return Descriptor;
}

/*
* Se le pasa un socket de servidor y acepta en el una conexion de cliente.
* devuelve el descriptor del socket del cliente o -1 si hay problemas.
*/
int aceptaConexionCliente(int descriptor)
{
	int longitudCliente;
	struct sockaddr_in cliente;
	int hijo;
	/*
	* La llamada a la funcion accept requiere que el parametro 
	* longitudCliente contenga inicialmente el tamano de la
	* estructura cliente que se le pase. A la vuelta de la
	* funcion, esta variable contiene la longitud de la informacion
	* util devuelta en cliente
	*/
	longitudCliente = sizeof(struct sockaddr_in);
	hijo = accept (descriptor, (struct sockaddr *)&cliente, &longitudCliente);
	if (hijo == -1){
		return -1;
	}
	/*
	* Se devuelve el descriptor en el que esta "enchufado" el cliente.
	*/
	return hijo;
}
