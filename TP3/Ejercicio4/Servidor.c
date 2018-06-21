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
#include "Socket_Servidor.h"
#include "Socket.h"

#define _GNU_SOURCE

typedef struct pthread_list{
    pthread_t               tid;
    struct pthread_list *   siguiente;
} t_pthread_list;

typedef struct {
    int               socket;
} pthread_par;

typedef struct pregunta{
    char pregunta[100];
    char respuestas[4][100];
    int rc;
    struct pregunta * siguiente;
} t_pregunta;

typedef struct{
    int seguir;
    char pregunta[100];
    char respuestas[4][100];
} t_pregunta_cliente;

t_pregunta *lista_preguntas = NULL;

int PUERTO = 10019;
int conectado = 1;
int Socket_Servidor = 0;
t_pthread_list * threads_list = NULL;
int jugando;

//---------------------------------------------------------------------------------------------------
void desconectar ();
void abrirArchivoDePreguntas();
void agregarPreguntaALista(char * pregunta, char * respuestas[], int respuestaCorrecta);
void iniciarServer();
void * atenderCliente (void * parametro);
void esperarThreads();
int obtenerSiguientePregunta(t_pregunta_cliente * pregunta, int nroPregunta);
//---------------------------------------------------------------------------------------------------

int main(int argc, char *const argv[]) {

    signal (SIGINT, desconectar);	//atiendo las signal
	signal (SIGTERM, desconectar);	//atiendo las signal

    abrirArchivoDePreguntas();
    iniciarServer();
    // exit(EXIT_SUCCESS);
}

//---------------------------------------------------------------------------------------------------
/*
*   Se encarga de leer el archivo de preguntas y cargar la lista.
*
*/
void abrirArchivoDePreguntas() {
    int cantidadDePreguntas = 0;
    int nroRespuesta = 0;

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("preguntas.txt", "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    t_pregunta *preguntas = lista_preguntas;

    int respuestasCorrectas = 0;
    int respuestaCorrecta = -1;
    char* resp[4];
    char* preg;

    int preguntaValida = 0;

    while ((read = getline(&line, &len, fp)) != -1) {
        
        //Obtengo el primer caracter, para saber si es pregunta o respuesta.
        char type = line[0];
        
        if( type == 'P'){
            //Siempre esta la pregunta antes de las respuestas.
            if( nroRespuesta != 4 || respuestasCorrectas != 1) {
                preguntaValida = 0;
            }
            if (preguntaValida) {

                //Si hubo una pregunta anterior y es válida, se debe agregar a la lista.
                agregarPreguntaALista(preg,resp,respuestaCorrecta);
                cantidadDePreguntas++;
                
                //Libero los punteros
                free(preg);
                for(int i=0; i<4;i++) {
                    free(resp[i]);
                }
            }
            respuestaCorrecta = -1;
            respuestasCorrectas = 0;
            nroRespuesta = 0;
            preguntaValida = 1;
            preg = (char*)malloc(strlen(line + 2) * sizeof(char));
            if(preg == NULL){
	            printf("Error, no hay mas memoria\n");
                exit(EXIT_FAILURE);
	        }

            strcpy(preg,line + 2);

        } else if ( type == 'R' ) {
            if(nroRespuesta > 3){
                preguntaValida = 0;
            } else {
                int offset = 2;
                if( line[1] == 'C') {    //Respuesta Correcta
                    respuestaCorrecta = nroRespuesta;
                    respuestasCorrectas++;
                    offset = 3;
                }

                resp[nroRespuesta] = (char*)malloc(strlen(line + offset) * sizeof(char));
                if(resp[nroRespuesta] == NULL){
	                printf("Error, no hay mas memoria\n");
                    exit(EXIT_FAILURE);
	            }

                strcpy(resp[nroRespuesta],line + offset);
                nroRespuesta++;
            }
        } else {
            //Basura y se ignora. (No es ni Pregunta ni respuesta)
        }
    }
    
    if( nroRespuesta != 4 || respuestasCorrectas != 1) {
        preguntaValida = 0;
    }

    //Se agrega la última pregunta del archivo.
    if (preguntaValida) {
        agregarPreguntaALista(preg,resp,respuestaCorrecta);
        cantidadDePreguntas++;
    }         
    //Libero los punteros
    free(preg);
    for(int i=0; i<4;i++) {
        free(resp[i]);
    }

    fclose(fp);
    if (line) {
        free(line);
    }
    printf("Cantidad de Preguntas: %d\n\n", cantidadDePreguntas);

    // t_pregunta *punteroLista = lista_preguntas;

    // int pregu = 1;
    // while(punteroLista != NULL){
    //         printf("Pregunta %d: %s",pregu, punteroLista->pregunta);
    //         for(int i=0; i<4;i++){
    //             printf("Respuesta %d: %s",i+1, punteroLista->respuestas[i]);
    //         }
    //         printf("\n");
    //         pregu++;
    //         punteroLista = punteroLista->siguiente;
    // }
}


//---------------------------------------------------------------------------------------------------
/*
*   Se encarga de agregar las preguntas a la lista.
*
*/
void agregarPreguntaALista(char * pregunta, char * respuestas[], int respuestaCorrecta) {
    t_pregunta *punteroLista = lista_preguntas;
    
    if (punteroLista == NULL) { //Primer Pregunta, lista vacía.
        
        //Inicializo la lista
        punteroLista = malloc(sizeof(t_pregunta));
        if(punteroLista == NULL){
	        printf("Error, no hay mas memoria\n");
            exit(EXIT_FAILURE);
	    }

        punteroLista->siguiente = NULL;
        lista_preguntas = punteroLista;
    } else {
        //Busco la ultima pregunta
        t_pregunta *ultimaPregunta = lista_preguntas;
        while(punteroLista != NULL) {
            ultimaPregunta = punteroLista;
            punteroLista = punteroLista->siguiente;
        }
        punteroLista = malloc(sizeof(t_pregunta));
        if(punteroLista == NULL){
	        printf("Error, no hay mas memoria\n");
	        exit(EXIT_FAILURE);
	    }
        punteroLista->siguiente = NULL;
        ultimaPregunta->siguiente = punteroLista;
    }

    //Cargo la pregunta
    strcpy(punteroLista->pregunta,pregunta);
    punteroLista->rc = respuestaCorrecta;
    
    //Cargo las respuestas
    for(int i=0; i<4;i++) {
        strcpy(punteroLista->respuestas[i],respuestas[i]);
    }
}

//---------------------------------------------------------------------------------------------------
/*
*   Se encarga de inicializar el server
*
*/

void iniciarServer(){
    pthread_t tid;

	t_pthread_list * ptl = threads_list;		//creo punteros para moverme por la lista
    t_pthread_list * ptlAnterior = NULL;		

    Socket_Servidor = abreSocket(NULL,PUERTO,2);		//Se abre el Socket del servidor
	if (Socket_Servidor == -1){
		printf ("No se puede abrir socket servidor\n");
	    exit(EXIT_FAILURE);
	}

    while (conectado) {					//mientras este conectado el socket y no se reciba la signal se tratara de conectar
		int Socket_Cliente = 0;

		Socket_Cliente = aceptaConexionCliente(Socket_Servidor);	//Se espera a que un cliente se conecte
        if (Socket_Cliente == -1) {
            if (conectado)							//Si aun debe tener conexion y falla es porque fallo el cliente
                printf ("No se puede abrir socket de cliente\n");
            else{
                printf ("\tCerrando sistema y subprocesos\n");	//Si falla porque se desconecto es por pedido de cerrar
            }
        } else {
            	if (ptl == NULL) {						//Si el puntero apunta a null creo un espacio de memoria para la informacion
	            	ptl= malloc(sizeof(t_pthread_list));		//Con esto creo al lista de threads
	            	if(ptl == NULL){
	                	printf("Error, no hay mas memoria\n");
	                    exit(EXIT_FAILURE);
	            	}
	        	}

            pthread_par par;
			par.socket = Socket_Cliente;

        	pthread_create (&tid, NULL, atenderCliente, (void *)&par);	//Creo el thread que se encargue de atender al cliente
            ptl->tid = tid;			//Guardo el thread id en la lista
        	
            /*
				Lo siguiente son algoritmos para hacer la lista dinamica
			*/
			ptl->siguiente = NULL;
        
        	if (ptlAnterior) {
            	ptlAnterior->siguiente = ptl;
        	}
        	ptlAnterior = ptl;
        	ptl = NULL;
		}
    }

    if (threads_list != NULL) {
    	esperarThreads();				//Espero la finalizacion de los threads creados
    }

	close (Socket_Servidor);					//Cierro el Socket del servidor
    
	printf("Finalizado el juego.\n");
    
	exit(EXIT_SUCCESS);	

}

//---------------------------------------------------------------------------------------------------
/*
*   Se encarga de atender a los clientes.
*
*/

void * atenderCliente (void * parametro){
    
    int respuesta;
 	pthread_t thread;
    int Socket_Cliente = ((pthread_par *)parametro)->socket;

    t_pregunta_cliente pregunta;
    pregunta.seguir = 4;

    int preguntaNro = 0;

    while(jugando){
        // if (
                strcpy(pregunta.pregunta,lista_preguntas->pregunta);
                for(int i = 0; i < 4; i++){
                    strcpy(pregunta.respuestas[i],lista_preguntas->respuestas[i]);
                }

            // obtenerSiguientePregunta(&pregunta, preguntaNro); //== 1){
            escribeSocket(Socket_Cliente, &pregunta, sizeof(pregunta));

            leeSocket (Socket_Cliente, &respuesta, sizeof(int));

        // } else {
        //     pregunta.seguir = 2;
        //     escribeSocket (Socket_Cliente, &pregunta, sizeof(pregunta));
        // }
        preguntaNro++;
    }   


    int resultado = 1;

    // escribeSocket (Socket_Cliente, &resultado, sizeof(int));

    /*
    * Se cierran el socket del cliente
    */
    close(Socket_Cliente);
}

int obtenerSiguientePregunta(t_pregunta_cliente * pregunta, int nroPregunta){
    int pos = 0;
    t_pregunta * pLista = lista_preguntas;
    while(pos < nroPregunta && pLista != NULL){
        pLista = pLista->siguiente;
        pos++;
    }

    if (pLista==NULL){
        return 0;
    }

    strcpy(pregunta->pregunta,pLista->pregunta);
    for(int i = 0; i < 4; i++){
        strcpy(pregunta->respuestas[i],pLista->respuestas[i]);
    }
    return 1;
}


//---------------------------------------------------------------------------------------------------
/*
*   Se encarga de esperar los threads de la lista.
*
*/
void esperarThreads(){
    int i =0;
    t_pthread_list * aux = NULL;
    
    pthread_t tid = 0;			//mediante la lista de threads ID voy recorriendo y revisando que hayan finalizado
    
    while (threads_list != NULL) {
        tid = threads_list->tid;
        
        aux = threads_list;
        if(aux->siguiente != NULL){
            threads_list = aux->siguiente;
        }else{
            threads_list = NULL;
	    }
	    if(tid!=0) {
            pthread_join(tid, NULL);	//espero la finalizacion del thread
        }   
        free(aux);			//libero la memoria que utlizo la lista
    }
    return;
}

//---------------------------------------------------------------------------------------------------
/*
 *  Esta funcion se encarga de hacer que no se acepten mas conexiones
 */

void desconectar (){
    printf ("\tCerrando el socket\n");

	conectado = 0;		//hago que se cierre la conexion
    close (Socket_Servidor);	//cierro el socket del servidor
}
