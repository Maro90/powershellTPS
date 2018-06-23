//
//  Socket.c
//  
//
//  Funciones de lectura y escritura en sockets
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/*
 * Lee datos del socket. Supone que se le pasa un buffer con hueco
 *	suficiente para los datos. Devuelve el numero de bytes leidos o
 * 0 si se cierra fichero o -1 si hay error.
 */
int leeSocket (int fd, void *datos, int longitud)
{
	int leido=0;
	int aux = 0;
    
	/*
     * Comprobacion de que los parametros de entrada son correctos
     */

	if (fd == -1)
		return -1;
    
	/*
     * Mientras no hayamos leido todos los datos solicitados
     */
//	while (leido < longitud)
//	{
		aux = recv (fd, datos, longitud,MSG_WAITALL);
		if (aux > 0)
		{
			/*
             * Si hemos conseguido leer datos, incrementamos la variable
             * que contiene los datos leidos hasta el momento
             */
		}
		else
		{
			/*
             * Si read devuelve 0, es que se ha cerrado el socket. Devolvemos
             * los caracteres leidos hasta ese momento
             */
			if (aux == 0)
				return leido;
			if (aux == -1)
			{
				/*
                 * En caso de error, la variable errno nos indica el tipo
                 * de error.
                 * El error EINTR se produce si ha habido alguna
                 * interrupcion del sistema antes de leer ningun dato. No
                 * es un error realmente.
                 * El error EGAIN significa que el socket no esta disponible
                 * de momento, que lo intentemos dentro de un rato.
                 * Ambos errores se tratan con una espera de 100 microsegundos
                 * y se vuelve a intentar.
                 * El resto de los posibles errores provocan que salgamos de
                 * la funcion con error.
                 */
				switch (errno)
				{
					case EINTR:
					case EAGAIN:
						usleep (100);
						break;
					default:
						return -1;
				}
			}
//		}
	}
    
	/*
     * Se devuelve el total de los caracteres leidos
     */
	return leido;
}

/*
 * Escribe dato en el socket cliente. Devuelve numero de bytes escritos,
 * o -1 si hay error.
 */
int escribeSocket (int fd, void *datos, int longitud)
{
	int escrito = 0;
	int aux = 0;
    
	/*
     * Comprobacion de los parametros de entrada
     */
	if ((fd == -1) || (datos == NULL) || (longitud < 1))
		return -1;
    
	/*
     * Bucle hasta que hayamos escrito todos los caracteres que nos han
     * indicado.
     */
//	while (escrito < longitud)
//	{
		aux = send (fd, datos + escrito, longitud - escrito,0);
		if (aux > 0)
		{
			/*
             * Si hemos conseguido escribir caracteres, se actualiza la
             * variable Escrito
             */
			escrito = escrito + aux;
		}
		else
		{
			/*
             * Si se ha cerrado el socket, devolvemos el numero de caracteres
             * leidos.
             * Si ha habido error, devolvemos -1
             */
			if (aux == 0)
				return escrito;
			else
				return -1;
		}
//	}
    
	/*
     * Devolvemos el total de caracteres leidos
     */
	return escrito;
}
