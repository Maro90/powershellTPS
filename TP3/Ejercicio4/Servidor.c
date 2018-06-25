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

//---------------------------------------------------------------------------------------------------

typedef struct pthread_list{
    pthread_t               tid;
    struct pthread_list *   siguiente;
} t_pthread_list;

//----------------------------

typedef struct {
    int               socket;
} pthread_par;

//----------------------------

typedef struct cliente{
    int                 socket;
    char                nombre[30];
    int                 puntos;
    int                 nroJugador;
    struct cliente  *   siguiente;
} t_clientes;

//----------------------------

typedef struct pregunta{
    char pregunta[100];
    char respuestas[4][100];
    int rc;
    struct pregunta * siguiente;
} t_pregunta;

//----------------------------

typedef struct{
    int seguir;
    char pregunta[100];
    char respuestas[4][100];
} t_comunicacion;


//---------------------------------------------------------------------------------------------------
int tiempoParaRegistrarse = 5;//segundos
int PUERTO = 10019;
int registrando = 1;
int Socket_Servidor = 0;

pthread_t   threadAlarma;
pthread_t   threadRegistro;

t_clientes *    lista_clientes = NULL;
t_pregunta *    lista_preguntas = NULL;
t_pthread_list * threads_list = NULL;

//---------------------------------------------------------------------------------------------------

void desconectar ();
int agregarPreguntaALista(char * pregunta, char * respuestas[], int respuestaCorrecta);
void * atenderCliente (void * parametro);
void esperarThreads();
int obtenerSiguientePregunta(t_comunicacion * pregunta, int nroPregunta);
int inicializar();
int abrirArchivoDePreguntas();
int levantarServer();
void *registrarClientes(void *arg);
void *FuncionAlarma(void *arg);
void handlerAlarma(int sig);
void iniciarJuego();

//---------------------------------------------------------------------------------------------------



int main(int argc, char *argv []) {

    signal (SIGINT, desconectar);	//atiendo las signal
	signal (SIGTERM, desconectar);	//atiendo las signal
	signal (SIGALRM, handlerAlarma);

    if (inicializar() == 0){
	    printf("No se pudo inicializar el juego el juego.\n");
        exit(EXIT_FAILURE);
    }

	pthread_create(&threadAlarma, NULL, FuncionAlarma, 0);
	
    pthread_create(&threadRegistro, NULL, registrarClientes, 0);

	pthread_join(threadAlarma,NULL);

    printf("Se acabo el tiempo de registro\n");
    iniciarJuego();

    if (threads_list != NULL) {
    	esperarThreads();
    }

	close (Socket_Servidor);
    
	printf("Finalizado el juego.\n");

}

//---------------------------------------------------------------------------------------------------

int inicializar(){

	lista_clientes = malloc(sizeof(t_clientes));
	if(lista_clientes == NULL){
        	printf("Error, no hay mas memoria\n");
		return EXIT_FAILURE;
	}

    threads_list = malloc(sizeof(t_pthread_list));
	if(threads_list == NULL){
        	printf("Error, no hay mas memoria\n");
		return EXIT_FAILURE;
	}



    if (abrirArchivoDePreguntas() == 0 ) {
        return 0;
    }
    if (levantarServer() == 0 ) {
        return 0;
    }
    printf("TODO LISTO PARA EMPEZAR\n");
    return 1;
}

//---------------------------------------------------------------------------------------------------

void *FuncionAlarma(void *arg){
	signal(SIGALRM, handlerAlarma);
	while(registrando){
		alarm(1); 
		sleep(1); 
	}
}
//---------------------------------------------------------------------------------------------------

void handlerAlarma(int sig){
    if (tiempoParaRegistrarse  == 0){
        pthread_cancel(threadAlarma);
        pthread_cancel(threadRegistro);
        registrando = 0;
    } else {
        tiempoParaRegistrarse--;
    }
}

//---------------------------------------------------------------------------------------------------

int levantarServer(){
    	struct sockaddr_in direccion;

	Socket_Servidor = socket(AF_INET, SOCK_STREAM, 0);
	if (Socket_Servidor == -1)
	 	return 0;

	direccion.sin_family = AF_INET;
	direccion.sin_addr.s_addr=htonl(INADDR_ANY);

    direccion.sin_port = htons(PUERTO);
	bzero(&(direccion.sin_zero), 8);

	if (bind(Socket_Servidor,(struct sockaddr *)&direccion,sizeof(struct sockaddr)) == -1){
		close(Socket_Servidor);
		printf("Error bind\n");
		return 0;
	}
	/*
	* Avisamos al sistema que comience a atender peticiones de clientes.
	*/
	if (listen(Socket_Servidor, 5) == -1) {
		close (Socket_Servidor);
		return 0;
	}

    return 1;
}


//---------------------------------------------------------------------------------------------------
/*
*   Se encarga de registrar a los clientes.
*
*/

void *registrarClientes(void *arg){

	t_clientes * pcl = lista_clientes;
    t_clientes * pclAnterior = NULL;		

    int nroJugador = 0;

    printf ("Comienza el registro\n");

    while (registrando) {   //Durante el periodo de registro
		int Socket_Cliente = 0;

		struct sockaddr Cliente; 
		int Longitud_Cliente; 

		Socket_Cliente = accept (Socket_Servidor, &Cliente, &Longitud_Cliente); 
        if (Socket_Cliente == -1) {
            if (registrando)							
                printf ("No se puede abrir socket de cliente\n");
            else{
                printf ("\tCerrando sistema y subprocesos\n");
            }
        } else {
            nroJugador++;

            if (pcl == NULL) {
	            pcl= malloc(sizeof(t_clientes));
	            if(pcl == NULL){
	                printf("Error, no hay mas memoria\n");
                    return 0;
	            }
	        }

            //Leo el nombre del cliente y lo guardo en el listado
            char nombre[30];
            Lee_Socket(Socket_Cliente, &nombre, 30 * sizeof(char));

            printf("%s se unió a la partida\n", nombre);

            pcl->socket = Socket_Cliente;
            pcl->puntos = 0;
            pcl->nroJugador = nroJugador;
            stpcpy(pcl->nombre,nombre);
            pcl->siguiente = NULL;
        
            if (pclAnterior) {
                pclAnterior->siguiente = pcl;
            }
            pclAnterior = pcl;
            pcl = NULL;
		}
    }
    printf("Basta de regustrar\n");

}

//---------------------------------------------------------------------------------------------------
/*
*   Se encarga de iniciar el juego, una vez terminado el tiempo de registro
*   Creara los threads requeridos para atender a todos los clientes.
*
*/


void iniciarJuego(){

	t_clientes * pcl = lista_clientes;

	t_pthread_list * ptl = threads_list;		//creo punteros para moverme por la lista
    t_pthread_list * ptlAnterior = NULL;		

    printf("A JUGAR\n");

    while(pcl != NULL ){

        if (ptl == NULL) {
	        ptl= malloc(sizeof(t_pthread_list));
            if(ptl == NULL){
                printf("Error, no hay mas memoria\n");
                exit(EXIT_FAILURE);
	        }
	    }

        pthread_par par;
	    par.socket = pcl->socket;

        pthread_t tid;
        pthread_create (&tid, NULL, atenderCliente, (void *)&par);
        ptl->tid = tid;
    
	    ptl->siguiente = NULL;
    
        if (ptlAnterior) {
            ptlAnterior->siguiente = ptl;
        }
        ptlAnterior = ptl;
        ptl = NULL;

        pcl = pcl->siguiente;
    }
    printf("TODOS LOS THREADS INICIADOS\n");
}




//---------------------------------------------------------------------------------------------------
/*
*   Se encarga de atender a los clientes.
*
*/

void * atenderCliente (void * parametro){
    
 	pthread_t thread;
    int Socket_Cliente = ((pthread_par *)parametro)->socket;

    char Cadena[100];
	int nroPregunta = 0;

    int hayMasPreguntas = 0;//1;
	t_comunicacion pregunta;
    pregunta.seguir = 1;
    int respuesta;

    while ( hayMasPreguntas ) {

        hayMasPreguntas = obtenerSiguientePregunta(&pregunta, nroPregunta);

	    Escribe_Socket(Socket_Cliente, &pregunta, sizeof(t_pregunta));
        // sleep(1);
        Lee_Socket(Socket_Cliente, &respuesta, sizeof(int));
        printf("Respondio: %d",respuesta);

    }
    
    pregunta.seguir = 0;
    Escribe_Socket (Socket_Cliente, &pregunta, sizeof(t_pregunta));

    /*
    * Se cierran el socket del cliente
    */
    close(Socket_Cliente);
    return NULL;
}

//---------------------------------------------------------------------------------------------------
/*
*   Se encarga de esperar los threads de la lista.
*
*/
void esperarThreads(){
    int i =0;
    t_pthread_list * aux = NULL;
    
    printf("Espera los threads\n");

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

	registrando = 0;		//hago que se cierre la conexion
    close (Socket_Servidor);	//cierro el socket del servidor
}



//---------------------------------------------------------------------------------------------------
/*
*   Se encarga de leer el archivo de preguntas y cargar la lista.
*
*/
int abrirArchivoDePreguntas() {
    int cantidadDePreguntas = 0;
    int nroRespuesta = 0;

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("preguntas.txt", "r");
    if (fp == NULL) {
        printf("No se encontro el archivo de preguntas \n");
        return 0;
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
                if(agregarPreguntaALista(preg,resp,respuestaCorrecta) == 0){
	                printf("No se pueden levantar las preguntas.\n");
                    return 0;
	            }
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
                return 0;
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
                    return 0;
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
    printf("Se cargaron las preguntas.\nCantidad de Preguntas: %d\n\n", cantidadDePreguntas);

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
int agregarPreguntaALista(char * pregunta, char * respuestas[], int respuestaCorrecta) {
    t_pregunta *punteroLista = lista_preguntas;
    
    if (punteroLista == NULL) { //Primer Pregunta, lista vacía.
        
        //Inicializo la lista
        punteroLista = malloc(sizeof(t_pregunta));
        if(punteroLista == NULL){
	        printf("Error, no hay mas memoria\n");
            return 0;
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
            return 0;
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
    return 1;
}

//---------------------------------------------------------------------------------------------------

int obtenerSiguientePregunta(t_comunicacion * pregunta, int nroPregunta){

    int pos = 0;
    t_pregunta * pLista = lista_preguntas;
    while(pos < nroPregunta && pLista != NULL){
        pLista = pLista->siguiente;
        pos++;
    }

    if (pLista==NULL){
        pregunta->seguir = 0;
        return 0;
    }

    strcpy(pregunta->pregunta,pLista->pregunta);
    for(int i = 0; i < 4; i++){
        strcpy(pregunta->respuestas[i],pLista->respuestas[i]);
    }
    return 1;
}