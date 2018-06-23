#include "Socket_Servidor.h"
#include "Socket.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>


/***********************    ESTRUCTURAS     ***********************/
struct pthread_list {
    pthread_t               tid;
    struct pthread_list *   siguiente;
};

struct pthread_par {
    int               socket;
};

struct coches {
    int             nro_coche;
    int             zona_actual;
    int             estado;
    struct coches * siguiente;
};

struct pedido {
    int             zona_inicial;
    int             zona_final;
};

struct respuesta {
    int             coche_asignado;
    int             tiempo;
};

struct pthread_parametros {
    int               tiempo;
    struct coches *   coche;
};


/***********************    VARIABLES     ************************/
pthread_mutex_t mutex_listado;
int conectado = 1;
int nro_vehiculos=0;
int Socket_Servidor = 0;
struct coches * vehiculos = NULL;
FILE *pf;
struct pthread_list * threads_list = NULL;


/***********************    FUNCIONES     ************************/
void    esperarThreads();
void    cancelarThreads(struct pthread_list *);
void    desconectar ();
void *  thread (void *);
int     elegirCoche(int, int, int, pthread_t *);
void    mostrarEstadoActual (int, int, int, int);
void *  liberarCoche(void *);
int     cargarArchivo(struct coches *);
int     actualizarArchivo();


/***********************    MAIN    ***** ************************/
int main (int argv, char argc []){

	signal (SIGINT, desconectar);	//atiendo las signal
	signal (SIGTERM, desconectar);	//atiendo las signal

	pthread_t tid;
	
	//Creamos el semaforo para threads de tipo mutex que utilizaremos
	pthread_mutex_init(&mutex_listado, NULL);
    
	threads_list= malloc(sizeof(struct pthread_list));	//creo el puntero a la lista de threads
	if(threads_list == NULL){
        	printf("Error, no hay mas memoria\n");
		return EXIT_FAILURE;
	}
    
    
	vehiculos = malloc(sizeof(struct coches));		//creo el puntero a la lista de vehiculos
	if(vehiculos == NULL){
		printf("Error, no hay mas memoria\n");
		return EXIT_FAILURE;
	}
    
   
	nro_vehiculos = cargarArchivo(vehiculos);		//cargo el archivo con los coches y sos zonas iniciales
	if (nro_vehiculos == -1) {
		printf("Error con el archivo Vehiculos.txt\n");
		return EXIT_FAILURE;
	}
   
 
	Socket_Servidor = abreSocket(NULL,10013,2);		//Se abre el Socket del servidor
	if (Socket_Servidor == -1)
	{
		printf ("No se puede abrir socket servidor\n");
		return EXIT_FAILURE;
	}
    
	struct pthread_list * ptl = threads_list;		//creo punteros para moverme por la lista
	struct pthread_list * ptlAnterior = NULL;		

	while (conectado) {					//mientras este conectado el socket y no se reciba la signal se tratara de conectar
		
		int Socket_Cliente = 0;

		Socket_Cliente = aceptaConexionCliente(Socket_Servidor);	//Se espera a que un cliente se conecte
        	if (Socket_Cliente == -1)
        	{
            		if (conectado)							//Si aun debe tener conexion y falla es porque fallo el cliente
                		printf ("No se puede abrir socket de cliente\n");
            		else{
                		printf ("\tCerrando sistema y subprocesos\n");	//Si falla porque se desconecto es por pedido de cerrar
            		}
        	}else{
        
	        	if (ptl == NULL) {						//Si el puntero apunta a null creo un espacio de memoria para la informacion
	            		ptl= malloc(sizeof(struct pthread_list));		//Con esto creo al lista de threads
	            		if(ptl == NULL){
	                		printf("Error, no hay mas memoria\n");
	                		return EXIT_FAILURE;
	            		}

	        	}
			struct pthread_par par;
			par.socket = Socket_Cliente;

        		pthread_create (&tid, NULL, thread, (void *)&par);	//Creo el thread que se encargue de atender al cliente
        
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
    
    	if (actualizarArchivo() == -1) {			//Actualizo el archivo de zona y vehiculos
        	printf("Error al actualizar el archivo Vehiculos.txt\n");
        	return EXIT_FAILURE;
    	}
	printf("Finalizado el Servicio de Remises. Buen dia :)\n");
    
   	pthread_mutex_destroy(&mutex_listado);				//Elimino el semaforo de threads
	return EXIT_SUCCESS;				
}


/*
 *  Esta funcion se encargara de cancelar los threads creados
 *  Ademas se encargara de liberar la memoria de la lista de threads
 *  Recibe por parametro el puntero a la lista de threads
 */

void esperarThreads(){
   

    int i =0;
    struct pthread_list * aux = NULL;
    
    pthread_t tid = 0;			//mediante la lista de threads ID voy recorriendo y revisando que hayan finalizado
    
    while (threads_list != NULL) {
        tid = threads_list->tid;
        
        aux = threads_list;
        if(aux->siguiente != NULL){
            threads_list = aux->siguiente;
        }else{
            threads_list = NULL;
	}
	if(tid!=0)
        	pthread_join(tid, NULL);	//espero la finalizacion del thread
        free(aux);			//libero la memoria que utlizo la lista
    }
    return;
}

/*
 *  Esta funcion se encarga de hacer que no se acepten mas conexiones
 */

void desconectar (){
	conectado = 0;		//hago que se cierre la conexion
    close (Socket_Servidor);	//cierro el socket del servidor
}


/*
 *  Esta funcion se encarga de atender a los clientes que soliciten pedidos de viaje
 *  Se ocupara de atender el pedido e iniciar el proceso de seleccion del coche
 *  Recibe por parametro el socket del cliente que hace el pedido.
 *  Tambien le respondera al cliente el nro de coche que se le asigno y el tiempo que demorara el viaje
 */

void * thread (void * parametro){
    
    	struct pedido pedidoCliente;
 	pthread_t thread;
	int tiempoDeViaje;
	int coche;
    	int Socket_Cliente = ((struct pthread_par *)parametro)->socket;
    	leeSocket (Socket_Cliente, &pedidoCliente, sizeof(struct pedido));

	if(pedidoCliente.zona_inicial >10 || pedidoCliente.zona_final >10){
	 	//no trabajamos con coches en zonas mayores a 10   	
		coche = -5;
	}else{
		pthread_mutex_lock(&mutex_listado);  //Evitamos que mas de un hilo utilice el listado de coches para que no asigne uno que se haya asignado.
    
    		srand(time(NULL));			//calculamos tiempo de viaje
    		tiempoDeViaje = rand()%10+5;
    
			//elegimos coche a partir de la zona inicial

    		coche = elegirCoche(pedidoCliente.zona_inicial, pedidoCliente.zona_final,tiempoDeViaje, &thread);
    
		//mostramos que esta haciendo el server
		mostrarEstadoActual(pedidoCliente.zona_inicial,pedidoCliente.zona_final,tiempoDeViaje,coche);

    		pthread_mutex_unlock(&mutex_listado);	//Liberamos el semaforo del thread una vez finalizado
	}
	
	struct respuesta seleccion;
    	if (coche == -1 || coche == -5) {
        	//no hay coche libre disponible
   		seleccion.coche_asignado = coche;
        	seleccion.tiempo = 0;
    	}
    	else{
        	seleccion.coche_asignado = coche;
        	seleccion.tiempo = tiempoDeViaje;
    	}
	//Se envia al cliente el coche y el tiempo que durara el recorrido
    	escribeSocket (Socket_Cliente, &seleccion, sizeof(struct respuesta));

    	/*
     	* Se cierran el socket del cliente
     	*/
    	close(Socket_Cliente);
	if(coche !=-1 && coche !=-5)
    		pthread_join(thread, NULL);	//se espera la finalizacion del thread
}

/*
 *  Esta funcion se encarga de buscar el coche que este en la zona solicitada
 *  Recibe por parametros la zona solicitada, la zona final y el tiempo de viaje
 *  Devolvera el nro del coche asignado
 */

int elegirCoche(int zona, int zona_final, int tiempoDeViaje, pthread_t * thread){

    	int coche = -1;
    	int encontrado = 0;
    	struct coches * p = vehiculos;
    	struct coches * aux;
        struct pthread_parametros * par;	//se crea la estructura con los parametros para el thread


    
    	while (encontrado == 0) {
        				//algoritmo de busqueda en la lista del coche que este en la zona solicitada y no este ocupado
            

        	if (p!=NULL) {
            		if(p->zona_actual==zona && p->estado == 0){

                		par = malloc(sizeof(struct pthread_parametros));
				//se asigna el coche se lo pasa a ocupado y se genera un thread que lo libere cuando finalice el recorrido
                		coche = p->nro_coche;
                		p->estado = 1;
                		p->zona_actual = zona_final;
                		encontrado = 1;
                		par->coche = p;
                		par->tiempo = tiempoDeViaje;
            		}
            		else{
                		aux = p->siguiente;
                		p = aux;
            		}
        	}
        	else
            		encontrado=1;
    	}
        if(coche != -1)
            pthread_create(thread, NULL, liberarCoche,(void *)par);
    			//se retorna el coche asigando
    	return coche;
}


/*
 *  Esta funcion se encarga de liberar un coche ocupado
 *  Recibe por parametro la cantidad de tiempo que debera ocupar el coche
 *      y el puntero al coche del listado que debera ocupar durante ese tiempo
 *  Al finalizar la funcion el coche estara libre
 */

void mostrarEstadoActual(int zona, int zona_final, int tiempoDeViaje, int coche){
	
	//se imprime por pantalla si se pudo asignar coche o se notifica lo contrario

	if (coche ==-1) {
		if(zona!=0 && zona_final!=0)
		{
        	printf("No hay coche disponible para viaje\n");
        	printf("Desde zona: %d    zona destino: %d\n",zona,zona_final);
        	printf("No hay coche en esa zona o esta ocupado en este momento.\n");
		}
        
    	}
    	else{
        	printf("Asignando pedido de viaje\n");
        	printf("Desde zona: %d    zona destino: %d\n",zona,zona_final);
        	printf("Coche seleccionado: %d\n",coche);
        	printf("Duracion del viaje: %d\n",tiempoDeViaje);
    	}
}


/*
 *  Esta funcion se encarga de liberar un coche ocupado
 *  Recibe por parametro la cantidad de tiempo que debera ocupar el coche
 *      y el puntero al coche del listado que debera ocupar durante ese tiempo
 *  Al finalizar la funcion el coche estara libre
 */

void * liberarCoche(void * parametros){
    
    struct coches * p = ((struct pthread_parametros *)parametros)->coche;
    int tiempoDeViaje = ((struct pthread_parametros *)parametros)->tiempo;

    	sleep(tiempoDeViaje);		//se estable la duracion del recorrido y luego se libera el coche
    
    pthread_mutex_lock(&mutex_listado);  //Evitamos que mas de un hilo utilice el listado de coches para que no asigne uno que se haya asignado.

    p->estado= 0;
    pthread_mutex_unlock(&mutex_listado);	//Liberamos el semaforo del thread una vez finalizado

    printf("El vehiculo %d se ha desocupado\n",p->nro_coche);

	free(parametros);
    
    	return;
}


/*
 *  Esta funcion se encarga de leer el archivo de coches
 *  Se ocupara de cargar los coches y sus zonas actuales en un listado
 *  Recibe como parametro el puntero a una listado
 *  Devuelve la cantidad de coches
 */

int cargarArchivo(struct coches * vehiculos){

    	int i =0;
    	struct coches * pVehic = NULL;
    	struct coches * pAnterior = NULL;
    	int vehiculo, zona;
	char buffer[100];
    
    	pVehic = vehiculos;
    
    	pf = fopen("Vehiculos.txt","r");	//se carga el archivo de coches en el listado
    	if (pf == NULL) {
        	return -1;
    	}

    	while (!feof(pf)) {
        	if (i==0){
            		fgets(buffer,100,pf);
            		fgets(buffer,100,pf);
       	 	}
        	if(fscanf(pf,"%d\t%d\n",&vehiculo,&zona) == 0)	//valida que el archivo cumpla con el formato indicado
			return -1;
        	if (pVehic==NULL) {
            		pVehic = malloc(sizeof(struct coches));
            		if (pVehic == NULL) {
                		printf("Error, no hay mas memoria\n");
            			return -1;
			}
        	}

		//algoritmo de generacion de la lista dinamica
        	pVehic->nro_coche = vehiculo;
        	pVehic->zona_actual = zona;
        	pVehic->estado = 0;
        	pVehic->siguiente = NULL;
        
        	if (pAnterior) {
            		pAnterior->siguiente = pVehic;
        	}
        	pAnterior = pVehic;
        	pVehic = NULL;
        	i++;
    	}
    
    	fclose(pf);	//se cierra el puntero al archivo
    
    	return i;   	//Retorna la cantidad de vehiculos
}


/*
 *  Esta funcion se actualizar el archivo de los coches con sus correspondientes zonas
 *  Se ocupara de liberar y borrar el listado de coches cargados en memoria
 *  Recibe como parametro el puntero a una listado
 *  Devuelve 1 si se actualizo correctamente y -1 si fallo;
 */

int actualizarArchivo(){
    
    	int i =0;
    	struct coches * aux = NULL;
    	int vehiculo, zona;
        
    	pf = fopen("Vehiculos.txt","w");	//se abre el archivo para actualizar los coches en que zonas quedaron
    	if (pf == NULL) {
    	    return -1;
    	}
    
    	fprintf(pf,"NUMERO DE VEHICULO | ZONA INICIAL\n");
    	fprintf(pf,"---------------------------------\n");
    
    	while (vehiculos != NULL) {

    	    vehiculo = vehiculos->nro_coche;
        	zona = vehiculos->zona_actual;
        	fprintf(pf,"%d \t %d\n",vehiculo,zona);

        	aux = vehiculos;
        	if(aux->siguiente != NULL){	
        	    vehiculos = aux->siguiente;
        	}else{
			vehiculos = NULL;
		}
        	free(aux);
    	}
    	fclose(pf);
    
    	return 1;
}


