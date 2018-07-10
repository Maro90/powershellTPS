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

/*#################################################
#			  Sistemas Operativos			 	#
#		Trabajo Práctico 3 - Ejericio 4	    	#
#		Nombre del Script: Servidor.c		    #
#												#
#				Integrantes:					#
#		Arana, Juan Pablo 		33904497		#
#		Gonzalez, Mauro Daniel 	35368160		#
#		Sapaya, Nicolás Martín 	38319489		#
#												#
#		Instancia de Entrega: 1er re Entrega	#
#												#
#################################################*/

#define SERVICIO_FIN_JUEGO 0
#define SERVICIO_NOMBRE_JUGADOR 1
#define SERVICIO_PREGUNTA 2
#define SERVICIO_TIEMPO 3
#define SERVICIO_FIN_PREGUNTAS 4
#define SERVICIO_FIN_REGISTRO 5
#define SERVICIO_REGISTRANDO 6

#define TIEMPO_RESPUESTA 10
#define TIEMPO_REGISTRO 10

//---------------------------------------------------------------------------------------------------

typedef struct {
    int               socket;
} pthread_par;

//----------------------------

typedef struct cliente{
    int                 socket;
    char                nombre[30];
    int                 puntos;
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
    int servicio;
    char pregunta[100];
    char respuestas[4][100];
} t_comunicacion;

//----------------------------

typedef struct{
    int servicio;
    int rta;
    char texto[30];
} t_respuesta_cliente;

typedef struct{
    int ultimo;
    int puntaje;
    char nombre[100];
} t_comunicacion_resultados;

//---------------------------------------------------------------------------------------------------

int tiempoParaRegistrarse = TIEMPO_REGISTRO;//segundos
int tiempoParaResponder = TIEMPO_RESPUESTA;

int PUERTO = 10016;
int registrando = 1;
int jugando = 0;
int Socket_Servidor = 0;
int finalizo = 0;
int cantidadDeJugadores = 0;
int respActual = 0;
int respPrimero = 1;

pthread_t   threadAlarma;
pthread_t   threadAlarma2;
pthread_t   threadRegistro;

t_clientes *	lista_ganadores = NULL;
t_clientes *    lista_clientes = NULL;
t_pregunta *    lista_preguntas = NULL;

fd_set descriptoresLectura;	/* Descriptores de interes para select() */

//---------------------------------------------------------------------------------------------------

void mostrarResultados();
void desconectar ();
int agregarPreguntaALista(char * pregunta, char * respuestas[], int respuestaCorrecta);
void esperarThreads();
int obtenerSiguientePregunta(t_comunicacion * pregunta, int nroPregunta);
int inicializar();
int abrirArchivoDePreguntas();
int levantarServer();
void *FuncionAlarma(void *arg);
void *FuncionAlarma2(void *arg);
void handlerAlarma(int sig);
void handlerAlarma2(int sig);
void iniciarJuego();
void agregarJugadorALista(int socket);
void borrarJugador(int descriptor);
void * atenderComunicaciones(void *arg);
void registrarNuevoCliente();
void mandarComunicacion(t_comunicacion comunicacion, int nroPreg);
void agregarGanador(t_clientes * winner);
void determinarGanadores();
void mostrarGanadores();
void mandarResultadosFinales();
void mandarResultado(t_comunicacion_resultados comunicacion);
void ordenarListaPorPuntaje();
void borrarJugadores();
//---------------------------------------------------------------------------------------------------

int main(int argc, char *argv []) {

    char hostName[100];
    gethostname(hostName,100);
    printf("\n\tPreguntanos en hostname: %s\n\n",hostName);


    if (argc != 2){
        printf("Error en la llamada, utilice -h para recibir más información.\n");
		exit(EXIT_FAILURE);
    }

	if (argc == 2 && (strcmp(argv[1], "-h") == 0  || strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-help") == 0) ){
		//hacer cosas de get-help
		printf("Modo de empleo: ./Servidor puerto \n");
		printf("ejemplo de ejecucion: ./Servidor 10005 \n");
		printf( " \n");
		printf( "Servidor levanta el juego preguntanos ");
		printf( "Posteriormente se deben levantar los clientes apuntando al mismo puerto y con la ip donde está corriendo el servidor.\n");
		printf( "\n");
		printf( " debe recibir como parametros: \n");
		printf( "	    -obligatorio: puerto que se utilizará para la comunicación con el cliente.\n");
		printf( "	    -opcional: h, -help muestra esta ayuda y finaliza \n");
		printf( "\n" );
		exit(0);
	}

    PUERTO = atoi(argv[1]);  //Puerto

    signal (SIGINT, desconectar);	//atiendo las signal
	signal (SIGTERM, desconectar);	//atiendo las signal
	signal (SIGALRM, handlerAlarma);

    if (inicializar() == 0){
	    printf("No se pudo inicializar el juego el juego.\n");
        exit(EXIT_FAILURE);
    }

	pthread_create(&threadAlarma, NULL, FuncionAlarma, 0);
    pthread_create(&threadRegistro, NULL, atenderComunicaciones, 0);
	pthread_join(threadAlarma,NULL);

    printf("Se acabo el tiempo de registro\n");

    if(cantidadDeJugadores > 0){
        iniciarJuego();
        mandarResultadosFinales();

        determinarGanadores();
    	mostrarGanadores();

    } else {
        printf("No hay jugadores\n");
        desconectar();
    }
	
        borrarJugadores();



    desconectar();

    pthread_join(threadRegistro,NULL);

	close (Socket_Servidor);
    
	printf("Finalizado el juego.\n");

}

//---------------------------------------------------------------------------------------------------

int inicializar(){

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

void *FuncionAlarma2(void *arg){
	signal(SIGALRM, handlerAlarma2);
	while(jugando){
		alarm(1); 
		sleep(1); 
	}
}

//---------------------------------------------------------------------------------------------------

void handlerAlarma(int sig){
    if (tiempoParaRegistrarse  == 0){
        pthread_cancel(threadAlarma);
        registrando = 0;
        jugando = 1;
    } else {
        tiempoParaRegistrarse--;
    }
}

//---------------------------------------------------------------------------------------------------

void handlerAlarma2(int sig){
    if (tiempoParaResponder  == 0){
        pthread_cancel(threadAlarma2);
    } else {
        tiempoParaResponder--;
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

void * atenderComunicaciones(void *arg){

    int nroJugador = 0;
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 50000;

    printf ("Comienza el registro\n");

    while(finalizo != 1){
        /* Se inicializa descriptoresLectura */

		FD_ZERO(&descriptoresLectura);

		/* Se añade para select() el socket servidor */

		FD_SET(Socket_Servidor, &descriptoresLectura);

		/* Se añaden para select() los sockets con los clientes ya conectados */
        t_clientes * pSockets = lista_clientes;
        int mayorDescriptor = 0;
        

        while(pSockets != NULL ){
			FD_SET(pSockets->socket, &descriptoresLectura);
            if (pSockets->socket > mayorDescriptor){
                mayorDescriptor = pSockets->socket;
            }
            pSockets = pSockets->siguiente;
        }

		if (mayorDescriptor < Socket_Servidor)
			mayorDescriptor = Socket_Servidor;

		/* Espera indefinida hasta que alguno de los descriptores tenga algo
		 * que decir: un nuevo cliente o un cliente ya conectado que envía un
		 * mensaje */
		select (mayorDescriptor + 1, &descriptoresLectura, NULL, NULL, &tv);

        t_clientes * pSockets2 = lista_clientes;
        
        while(pSockets2 != NULL){
			if (FD_ISSET (pSockets2->socket, &descriptoresLectura)) {
				/* Se lee lo enviado por el cliente y se escribe en pantalla */
                t_respuesta_cliente respuesta;
				if ((Lee_Socket(pSockets2->socket, &respuesta, sizeof(t_respuesta_cliente)) > 0)) {
                    switch(respuesta.servicio){
                        case SERVICIO_NOMBRE_JUGADOR:
                            //Nombre del jugador
                            printf("%s se unió a la partida\n", respuesta.texto);
                            strcpy(pSockets2->nombre,respuesta.texto);
                            break;
                        case SERVICIO_PREGUNTA:
                            //Respuesta a la pregunta
                            printf("%s respondio %d\n", pSockets2->nombre, respuesta.rta);
							if(respPrimero == 1){
								if(respuesta.rta == (respActual+1)){
									pSockets2->puntos++;
								}else if(respuesta.rta != -2){
									pSockets2->puntos--;
								}
								// SI QUIERO QUE CONTESTE 1 BIEN SI O SI, LO PONGO EN EL IF DE RTA CORRECTA, Y A LOS
								// QUE CONTESTA MAL LES RESTA 1, Y SIEMPRE A UNO LE VA A SUMAR
								respPrimero = 0;
                            }
                            break;
                        default:
                            break;
                    }

                    //TODO: LEER A LOS CLIENTES


				} else {
					/* El cliente ha cerrado la conexión y se elimina */
					printf ("Cliente ha cerrado la conexión\n");
                    borrarJugador(pSockets2->socket);                    
				}
			}
            pSockets2 = pSockets2->siguiente;
        }
        /* Se comprueba si algún cliente nuevo desea conectarse y se le
		 * admite */
		if (FD_ISSET(Socket_Servidor, &descriptoresLectura))
			registrarNuevoCliente();
    }

    return NULL;
}
//---------------------------------------------------------------------------------------------------

void registrarNuevoCliente(){
    int Socket_Cliente = 0;
	struct sockaddr Cliente; 
	socklen_t Longitud_Cliente;
    int len = sizeof(Cliente);

	Socket_Cliente = accept(Socket_Servidor, (struct sockaddr *)&Cliente, &len);
    if (Socket_Cliente == -1) {
        if (finalizo != 1)							
            printf ("No se puede abrir socket de cliente\n");
        else{
            printf ("\tCerrando sistema y subprocesos\n");
        }
    } else {
        if (registrando){
            agregarJugadorALista(Socket_Cliente);
            t_comunicacion comunicacion;
            comunicacion.servicio = SERVICIO_REGISTRANDO;
            Escribe_Socket (Socket_Cliente, &comunicacion, sizeof(t_comunicacion));

        } else{
            t_comunicacion comunicacion;
            comunicacion.servicio = SERVICIO_FIN_REGISTRO;
            Escribe_Socket (Socket_Cliente, &comunicacion, sizeof(t_comunicacion));
            close(Socket_Cliente);
        }
	}    
}

//---------------------------------------------------------------------------------------------------

void agregarJugadorALista(int socket){
    t_clientes * pcl = lista_clientes;
    t_clientes * pclAnterior = NULL;		
    
    if ( lista_clientes == NULL ){
        lista_clientes = malloc(sizeof(t_clientes));
	    if(lista_clientes == NULL){
	        printf("Error, no hay mas memoria\n");
            desconectar();
            exit(EXIT_FAILURE);
	    }
        lista_clientes->socket = socket;
        lista_clientes->puntos = 0;
        lista_clientes->siguiente = NULL;
        cantidadDeJugadores++;

        return;
    } 
    
    while(pcl != NULL ){
        pclAnterior = pcl;
        pcl = pcl->siguiente;
    }

    if (pcl == NULL) {
	    pcl= malloc(sizeof(t_clientes));
	    if(pcl == NULL){
	        printf("Error, no hay mas memoria\n");
            exit(EXIT_FAILURE);
	    }
	}
    
    pcl->socket = socket;
    pcl->puntos = 0;
    pcl->siguiente = NULL;
        
    if (pclAnterior) {
        pclAnterior->siguiente = pcl;
    }     
    cantidadDeJugadores++;

}

//---------------------------------------------------------------------------------------------------

void borrarJugador(int descriptor){
    t_clientes * pcl = lista_clientes;
    t_clientes * pclAnterior = pcl;		
    
    while(pcl != NULL && pcl->socket != descriptor){
        pclAnterior = pcl;
        pcl = pcl->siguiente;
    }

    if(pcl->socket == descriptor){
        pclAnterior->siguiente = pcl->siguiente;
        close(pcl->socket);
        free(pcl);
    } else {
        printf("No encontro ese cliente");
    }
    cantidadDeJugadores--;
    if(jugando == 1 && cantidadDeJugadores < 1){
        printf("Se desconectaron los jugadores, abortando juego\n");
        desconectar();
        return;
    }
}

//---------------------------------------------------------------------------------------------------
/*
*   Se encarga de iniciar el juego, una vez terminado el tiempo de registro
*   Creara los threads requeridos para atender a todos los clientes.
*
*/

void iniciarJuego(){

	t_clientes * pcl = lista_clientes;
	int nroPregunta = 0;

    printf("A JUGAR\n");
    int hayMasPreguntas = 1;
	t_comunicacion pregunta;
    pregunta.servicio = SERVICIO_PREGUNTA;
    int respuesta;

    while(jugando && hayMasPreguntas && cantidadDeJugadores > 0) {
        hayMasPreguntas = obtenerSiguientePregunta(&pregunta, nroPregunta);

        if(hayMasPreguntas == 1){
            mandarComunicacion(pregunta, nroPregunta);
	
            tiempoParaResponder = TIEMPO_RESPUESTA;
            pthread_create(&threadAlarma2, NULL, FuncionAlarma2, 0);
	        pthread_join(threadAlarma2,NULL);

            printf("Tiempo agotado, siguiente pregunta \n");
            pregunta.servicio = SERVICIO_TIEMPO;
            mandarComunicacion(pregunta, 0);
            sleep(8);   //Tiempo entre pregunta y pregunta.

            nroPregunta++;

        }
    }
    if( cantidadDeJugadores < 1) {
        return;
    }
    
    pregunta.servicio = SERVICIO_FIN_PREGUNTAS;
    mandarComunicacion(pregunta,nroPregunta);

    // pregunta.servicio = SERVICIO_FIN_JUEGO;
    // mandarComunicacion(pregunta,nroPregunta);
}

//---------------------------------------------------------------------------------------------------
/*
*   Se encarga de mandar la info a todos los clientes.
*
*/

void mandarComunicacion(t_comunicacion comunicacion, int nroPreg){
    t_clientes * clientes = lista_clientes; 
	
	if(nroPreg != 0){
        // Muestro resultado Parcial
        ordenarListaPorPuntaje();
        mostrarResultados();
		respPrimero = 1;
    }
	
    while(clientes != NULL){
        Escribe_Socket (clientes->socket, &comunicacion, sizeof(t_comunicacion));
        clientes = clientes->siguiente;
    }
}

//---------------------------------------------------------------------------------------------------
/*
 *  Esta funcion se encarga de hacer que no se acepten mas conexiones
 */

void desconectar (){
    finalizo = 1;		//hago que se cierre la conexion
    jugando = 0;

    pthread_cancel(threadAlarma);
    pthread_cancel(threadAlarma2);
    pthread_cancel(threadRegistro);



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
        pregunta->servicio = SERVICIO_FIN_PREGUNTAS;
        return 0;
    }

    strcpy(pregunta->pregunta,pLista->pregunta);
    for(int i = 0; i < 4; i++){
        strcpy(pregunta->respuestas[i],pLista->respuestas[i]);
    }
    pregunta->servicio = SERVICIO_PREGUNTA;
	respActual = pLista->rc;
    return 1;
}

//---------------------------------------------------------------------------------------------------

void mostrarResultados(){
    t_clientes * p = lista_clientes;

    if(p){
        printf("*******************************************************\n");
        printf("Resultados Parciales:\n\n");
    }

    while(p){
        printf("Nombre: %s\tPuntuación: %d\n", p->nombre, p->puntos);
        p = p->siguiente;
    }
    printf("*******************************************************\n");
}

//---------------------------------------------------------------------------------------------------

void mostrarGanadores(){
    t_clientes * p = lista_ganadores;

    if(p){
        printf("*******************************************************\n");
        printf("Ganadores:\n\n");
    }

    while(p){
        printf("Nombre: %s\tPuntuación: %d\n", p->nombre, p->puntos);
        p = p->siguiente;
    }
    printf("*******************************************************\n");
}

//---------------------------------------------------------------------------------------------------

void determinarGanadores(){
    t_clientes * p = lista_clientes;
	t_clientes * buscarMas = lista_clientes;
	t_clientes maximo;
	
	// Establezco el primero como máximo
	strcpy(maximo.nombre, p->nombre);
	maximo.socket = p->socket;
	maximo.puntos = p->puntos;
	
	// Muevo el puntero al segundo
	if(p->siguiente != NULL){
		p = p->siguiente;
	}
	
	// Si hay más de 1 jugador, busco entre los demás cual es el de máximo puntaje
    while(p){
        if(maximo.puntos < p->puntos){
			// Hay otro de mayor puntaje
			strcpy(maximo.nombre, p->nombre);
			maximo.socket = p->socket;
			maximo.puntos = p->puntos;
		}
        p = p->siguiente;
    }
	
	agregarGanador(&maximo);
	
	// Ahora me fijo si hay más de uno con el mismo puntaje
	while(buscarMas){
		if(maximo.puntos == buscarMas->puntos && maximo.socket != buscarMas->socket){
			// Hay otro de mayor puntaje
			strcpy(maximo.nombre, buscarMas->nombre);
			maximo.socket = buscarMas->socket;
			maximo.puntos = buscarMas->puntos;
			agregarGanador(&maximo);
		}
        buscarMas = buscarMas->siguiente;
	}
}

//---------------------------------------------------------------------------------------------------

void agregarGanador(t_clientes * winner){
    t_clientes * pcl = lista_ganadores;
    t_clientes * pclAnterior = NULL;		
    
    if ( lista_ganadores == NULL ){
        lista_ganadores = malloc(sizeof(t_clientes));
	    if(lista_ganadores == NULL){
	        printf("Error, no hay mas memoria\n");
            desconectar();
            exit(EXIT_FAILURE);
	    }
		
        lista_ganadores->socket = winner->socket;
		strcpy(lista_ganadores->nombre, winner->nombre);
        lista_ganadores->puntos = winner->puntos;
        lista_ganadores->siguiente = NULL;

        return;
    } 
    
    while(pcl != NULL ){
        pclAnterior = pcl;
        pcl = pcl->siguiente;
    }

    if (pcl == NULL) {
	    pcl = malloc(sizeof(t_clientes));
	    if(pcl == NULL){
	        printf("Error, no hay mas memoria\n");
            exit(EXIT_FAILURE);
	    }
	}
    
    pcl->socket = winner->socket;
    strcpy(lista_ganadores->nombre, winner->nombre);
    pcl->puntos = winner->puntos;
    pcl->siguiente = NULL;
        
    if(pclAnterior) {
        pclAnterior->siguiente = pcl;
    }     
}

//---------------------------------------------------------------------------------------------------

void mandarResultadosFinales(){
    t_comunicacion_resultados comunicacion;
    t_clientes * p = lista_clientes;

    while(p != NULL){
        strcpy(comunicacion.nombre,p->nombre);
        comunicacion.puntaje = p->puntos;
        if(p->siguiente == NULL){
            comunicacion.ultimo = 1;
        } else {
            comunicacion.ultimo = 0;
        }

        mandarResultado(comunicacion);
        p = p->siguiente;
    
    }
}

//---------------------------------------------------------------------------------------------------

void mandarResultado(t_comunicacion_resultados comunicacion){
    t_clientes * clientes = lista_clientes; 
	
	
    while(clientes != NULL){
        Escribe_Socket (clientes->socket, &comunicacion, sizeof(t_comunicacion_resultados));
        clientes = clientes->siguiente;
    }
}

//---------------------------------------------------------------------------------------------------

void ordenarListaPorPuntaje() {
     t_clientes * actual , * siguiente;
     int t;
     int extS;
     char ext[100];
     actual = lista_clientes;
     while(actual->siguiente != NULL) {
          siguiente = actual->siguiente;
          
          while(siguiente!=NULL) {
               if(actual->puntos < siguiente->puntos) {

                    t = siguiente->puntos;
                    siguiente->puntos = actual->puntos;
                    actual->puntos = t;   

                    extS = siguiente->socket;
                    siguiente->socket = actual->socket;
                    actual->socket = extS;   

                    strcpy(ext,siguiente->nombre);
                    strcpy(siguiente->nombre,actual->nombre);
                    strcpy(actual->nombre,ext);

               }
               siguiente = siguiente->siguiente;                    
          }    
          actual = actual->siguiente;
          siguiente = actual->siguiente;      
     }
}

//---------------------------------------------------------------------------------------------------
void borrarJugadores(){
    t_clientes * clientes = lista_clientes; 
	
    while(clientes != NULL){
        borrarJugador(clientes->socket);
        clientes = clientes->siguiente;
    }

}