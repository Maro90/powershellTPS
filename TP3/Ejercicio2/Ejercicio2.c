#include "parametros.h"
#include "lista.h"

/*#################################################
#			  Sistemas Operativos			 	#
#		Trabajo Práctico 3 - Ejericio 2			#
#		Nombre del Script: Ejercicio2.c		    #
#												#
#				Integrantes:					#
#		Arana, Juan Pablo 		33904497		#
#		Gonzalez, Mauro Daniel 	35368160		#
#		Sapaya, Nicolás Martín 	38319489		#
#												#
#		Instancia de Entrega: Entrega			#
#												#
#################################################*/

t_lista lista;
t_lista copiaLista;
long int caracArch = 0;

int main(int argc, char *const argv[]){
    int multiplicidad;
    int cantFiles = 0;
    int filesTotal = 0;
    int i = 0;
    int j = 0;

    t_pantalla pantalla;
    t_dat dato;

    if(argc != 2 && argc != 4){
        printf("Ingrese:\n\t./Ejercicio2 -h\n\t./Ejercicio2 -help\n\t./Ejercicio2 -?\n\tpara solicitar ayuda.\n");
        exit(1);
    } 
    if(argc == 2 && ((strcmp(argv[1],"-help")==0) || (strcmp(argv[1],"-h") == 0) || (strcmp(argv[1],"-?") == 0))){
        printf("La sintaxis es ./Ejercicio2 [pathEntrada] [pathSalida] [nivel paralelismo]\nFormato: [string] [string] [int]\nEjemplo: ./Entrada ./Salida 1\n");
        exit(1);
    }else if(argc != 4){
        printf("Ingrese:\n\t./Ejercicio2 -h\n\t./Ejercicio2 -help\n\t./Ejercicio2 -?\n\tpara solicitar ayuda.\n");
        exit(1);
    }
    if(argc == 4 && ((((int)*argv[3]) < 49) || (((int)*argv[3]) > 57))){ // compruebo que este entre 1 y 9
        printf("[nivel paralelismo] debe ser un número entre 1 y 9\n");
        exit(1);
    }

    // Inicializo variable de max y min caracteres
    pantalla.minCaracter = 999999999;
    pantalla.maxCaracter = 0;

    // (((int)*argv[3])-48) es el parametro 3, que sería la multiplicidad
    strcpy(dato.pathIn, argv[1]);
    strcpy(dato.pathOut, argv[2]);
    strcat(dato.pathIn, "/");
    strcat(dato.pathOut, "/");
    multiplicidad = (((int)*argv[3])-48);
    //////////////////////////////THREADS//////////////////////////////////////////////////////
    crearLista(&lista);
    crearLista(&copiaLista);
    cargarArchivosEnLista(&lista, dato.pathIn);
    
    filesTotal = countFiles(dato.pathIn);

    pthread_t hilos[multiplicidad];

    while(cantFiles < filesTotal){
		for (i = 0; i < size(&lista); i++){

            strcpy(dato.nombreArchivo, lista->dato.name);

            lista->dato.pidThread = j+1;
            insertarOrdenado(&copiaLista,&lista->dato,cmp);

			//Voy a procesar el primer archivo de la lista
			pthread_create(&hilos[j], NULL, analyze, (void*)&dato);

			//Se realiza una espera bloqueante a cada uno de los threads de vecTid[i]
			pthread_join(hilos[j], NULL);

            // Me fijo si el archivo recien procesado posee menor o mayor cant de caracteres totales que los demás
            comprobarMaxMin(&pantalla, dato.nombreArchivo, caracArch);

            if(cantFiles == 0){
                // Guardo el primer archivo completado
                strcpy(pantalla.primerArchivo, dato.nombreArchivo);
            }else if(cantFiles == filesTotal-1){
                // Guardo el último archivo completado
                strcpy(pantalla.ultimoArchivo, dato.nombreArchivo);
            }

			//Borro la posicion del archivo actual procesado
            eliminarPorPosicion(&lista, 1);
            
            cantFiles++;
			j++;

			//Si la cantidad de archivos, supera la cantidad maxima establecida de Threads, vuelvo al primer thread
			if(j>=multiplicidad){
				j=0;
            }
		}
	}
    
    ordenarLista(&copiaLista);
    mostrarLista(&copiaLista);
    imprimirPantalla(pantalla);

    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void* analyze(void* d){
    t_imprimir imprimir;

    FILE *fp;
    char caracter;
    char pin[100];
    char pout[100];
    time_t timeBegin;
    time_t timeEnd;
    t_dat dat = *(t_dat *) d;

    imprimir.pid = lista->dato.pidThread;
    imprimir.cantVocales = 0;
    imprimir.cantConsonantes = 0;
    imprimir.cantCaracteres = 0;
    strcpy(pin,dat.pathIn);
    strcpy(pout,dat.pathOut);
    strcat(pin,dat.nombreArchivo);
    strcat(pout,dat.nombreArchivo);

    fp = fopen(pin,"r");
    imprimir.pf = fopen(pout,"w");

    if(!fp){
        fputs("File IN error\n",stderr);
        exit(1);
    }
    if(!imprimir.pf){
        fputs("File OUT error\n",stderr);
        exit(1);
    }

    // Tomo el tiempo de inicio
    time(&timeBegin);
    imprimir.tInfo1 = localtime(&timeBegin);

    while((caracter = fgetc(fp)) != EOF){
        // es letra?
        if(ES_LETRA(caracter)){
             // Es vocal o consontante?
            if(ES_VOCAL(caracter)){
                // Es vocal
                imprimir.cantVocales++;
            }else{
                // Es consonante
                imprimir.cantConsonantes++;
            }
        }else{
            // Si es /n no lo cuento
            if(caracter != 10){
                imprimir.cantCaracteres++;
            }
        }
	}

    // Sumo para tener la cantidad de caracteres totales en caracArch
    caracArch = imprimir.cantCaracteres + imprimir.cantConsonantes + imprimir.cantVocales;

    // Tomo el tiempo de finalización
    time(&timeEnd);
    imprimir.tInfo2 = localtime(&timeEnd);

    // Imprimo todo el file
    printFile(imprimir);

    // Cierro file pointers
    fclose(imprimir.pf);
    fclose (fp);

    return(0);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////