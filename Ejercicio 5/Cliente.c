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



/***********************    ESTRUCTURAS     ***********************/


struct pedido {
    int             zona_inicial;
    int             zona_final;
};


struct respuesta {
    int             coche_asignado;
    int             tiempo;
};
/***********************    VARIABLES     ************************/

int Socket_Con_Servidor;
int coche_asignado;
/***********************    PROTOTIPOS FUNCIONES     ************************/
void desconectar ();
void capturador_seniales(int );

/***********************    MAIN    ***** ************************/
int main(int argc, char *argv [])
{
	if(argc !=2) /* Indica la sintaxis correcta a utilizar */
	{
		printf("ERROR. Sintaxis: %s < ip servidor > \n", argv[0] );
		return 1;
	}
	struct pedido  pedido_vehiculo;
	struct respuesta  respuesta_pedido_vehiculo;
	struct sigaction sig_action; 

	/* Posibles señales a recibir */

	sig_action.sa_handler=capturador_seniales;  
	sigaction(SIGTERM, &sig_action, NULL);
	sigaction(SIGINT, &sig_action, NULL);

	/* Creo la conexion con el servidor */
	Socket_Con_Servidor = abreConexion(argv[1], 10013);

	if (Socket_Con_Servidor == -1) /*Se verifica que se realizo correctamente la conexion */
	{
		printf ("No puedo establecer conexion con el servidor\n");
		return 1;
	}

	printf("*** PEDIDO NUEVO ***\n\n"); /* Realizo un pedido, se pide la zona inicial */
	printf("Ingrese Zona inicial: ");
	scanf("%d", &pedido_vehiculo.zona_inicial);
	while (pedido_vehiculo.zona_inicial<0) /* La zona inicial no puede ser negativa */
	{
		printf("No se admiten zonas negativas\n");
		printf("Ingrese Zona inicial: ");
		scanf("%d", &pedido_vehiculo.zona_inicial);
	}
	
	printf("Ingrese Zona Final: "); /* Realizo un pedido, se pide la zona final */
	scanf("%d", &pedido_vehiculo.zona_final);
	while (pedido_vehiculo.zona_final<0) /* La zona final no puede ser negativa */
	{
		printf("No se admiten zonas negativas\n");
		printf("Ingrese Zona final: ");
		scanf("%d", &pedido_vehiculo.zona_final);
	}

	escribeSocket (Socket_Con_Servidor, &pedido_vehiculo, sizeof(struct pedido)); /* Escribo el socket enviando la zona inicial y final */

	leeSocket (Socket_Con_Servidor, &respuesta_pedido_vehiculo, sizeof(struct respuesta)); /* Leo el sockeT obteniendo el coche utilizado y el tiempo */
	if(respuesta_pedido_vehiculo.tiempo == 0 && (respuesta_pedido_vehiculo.coche_asignado == -1 || respuesta_pedido_vehiculo.coche_asignado == -5)) /* Si el tiempo = 0 y el coche = -1*/
		if(respuesta_pedido_vehiculo.coche_asignado == -1)
			printf ("No hay vehiculo en la zona\n"); /*significa que no hay un auto disponible*/
		else
			printf ("Disculpe pero no trabajamos con coches esa zona solicitada\nNuestros vehiculos solo van y vienen entre las zonas 1 a 10\n"); /*significa que no se trabaja en una de las zonas pedidas*/
	else
		printf("El vehiculo %d\nZona inicial: %d\nZona final %d\nTiempo de: %d\n", respuesta_pedido_vehiculo.coche_asignado, 	pedido_vehiculo.zona_inicial, pedido_vehiculo.zona_final, respuesta_pedido_vehiculo.tiempo); /* indico el vehiculo utilizado para ir de la zona inicial a la final y el tiempo necesario */

	printf("\nSe cierra el socket cliente...\n");
	close (Socket_Con_Servidor); /* Cierro el socket */
	return 0;
}


/***********************    FUNCIONES     ************************/


//Finalización abrupta.
void capturador_seniales(int signum) 
{
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
void desconectar ()
{
	printf("\nEl cliente finalizo!\n");
	struct pedido  aux;
	aux.zona_final=0;
	aux.zona_inicial=0;
	escribeSocket (Socket_Con_Servidor, &aux, sizeof(struct pedido));
	close (Socket_Con_Servidor);
	exit(EXIT_FAILURE);
}


