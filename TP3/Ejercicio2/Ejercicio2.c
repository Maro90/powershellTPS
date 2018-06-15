#include "parametros.h"
#include "lista.h"

t_lista lista;

int main(int argc, char *const argv[]){
    int multiplicidad;
    char pathIn[100];
    char pathOut[100];
    int cantFiles = 0;
    int filesTotal = 0;
    int i = 0;
    int j = 0;

    t_dat dato;

    if(argc != 2 && argc != 4){
        printf("Ingrese ./Ejercicio2.exe h para ayuda.\n");
        exit(1);
    }else if(argc == 2 && (strcmp(argv[1],"h") == 0)){
        printf("La sintaxis es ./Ejercicio2.exe [pathEntrada] [pathSalida] [nivel paralelismo]\nFormato: [string] [string] [int]\nEjemplo: Entrada/ Salida/ 1\n");
        exit(1);
    }else if((((int)*argv[3]) < 49) || (((int)*argv[3]) > 57)){ // compruebo que este entre 1 y 9
        printf("[nivel paralelismo] debe ser un número entre 1 y 9\n");
        exit(1);
    }

    // (((int)*argv[3])-48) es el parametro 3, que sería la multiplicidad
    strcpy(dato.pathIn, argv[1]);
    strcpy(dato.pathOut, argv[2]);
    multiplicidad = (((int)*argv[3])-48);
    //////////////////////////////THREADS//////////////////////////////////////////////////////
    crearLista(&lista);
    cargarArchivosEnLista(&lista, dato.pathIn);
    mostrarLista(&lista);

    filesTotal = countFiles(dato.pathIn);

    //******************************************//
    printf(".....................................\n");
    printf("Files Totales = %d\n", filesTotal);
    //******************************************//

    pthread_t hilos[multiplicidad];

    while(cantFiles < filesTotal){
		for (i = 0; i < size(&lista); i++){

            strcpy(dato.nombreArchivo,lista->dato.name);
			//Voy a procesar el primer archivo de la lista
			pthread_create(&hilos[j], NULL, analyze, (void*)&dato);

            //******************************************//
            printf("Esperando Join\n");
            printf(".....................................\n");
            //******************************************//

			//Se realiza una espera bloqueante a cada uno de los threads de vecTid[i]
			pthread_join(hilos[j], NULL);

            //******************************************//
            printf(".....................................\n");
            printf("Sali del Join\n");
            printf(".....................................\n");
            //******************************************//

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
    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int countFiles(char pathIn[]){
    int file_count = 0;
    DIR * dirp;
    struct dirent * entry;

    dirp = opendir(pathIn);

    if(!dirp){
        printf("No se pudo abrir el directorio.\n");
        exit(1);
    }

    entry = readdir(dirp);

    while(entry != NULL) {
        if(entry->d_type == DT_REG){ // Si es archivo regular
            file_count++;
        }
        entry = readdir(dirp);
    }
    closedir(dirp);
    return(file_count);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void printFile(t_imprimir p){

    //******************************************//
    printf(".....................................\n");
    printf("Imprimiendo archivo\n");
    printf(".....................................\n");
    //******************************************//

    fprintf(p.pf, "%d:%d:%d\n", p.tInfo1->tm_hour, p.tInfo1->tm_min, p.tInfo1->tm_sec);
    fprintf(p.pf, "PID: %d\n",p.pid);
    fprintf(p.pf ,"Vocales:%d\nConsonantes:%d\nCaracteres:%d\n", p.cantVocales, p.cantConsonantes, p.cantCaracteres);
    fprintf(p.pf, "%d:%d:%d\n", p.tInfo2->tm_hour, p.tInfo2->tm_min, p.tInfo2->tm_sec);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void* analyze(void* d){
    t_imprimir imprimir;
    lista->dato.pidThread = getpid();
    FILE *fp;
    char caracter;
    char pin[100];
    char pout[100];
    time_t timeBegin;
    time_t timeEnd;
    t_dat dat = *(t_dat *) d;

    imprimir.pid = getpid();
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

    //******************************************//
    printf("\n-------------------------------------------\n");
    printf("Resultado:\nVocales:%d\nConsonantes:%d\nCaracteres:%d\n", imprimir.cantVocales, imprimir.cantConsonantes, imprimir.cantCaracteres);
    printf("\n-------------------------------------------\n");
    //******************************************//

    // Tomo el tiempo de finalización
    time(&timeEnd);
    imprimir.tInfo2 = localtime(&timeEnd);

    // Imprimo todo el file
    printFile(imprimir);

    //******************************************//
    printf(".....................................\n");
    printf("termine de imprimir\n");
    printf(".....................................\n");
    //******************************************//

    // Cierro file pointers
    fclose(imprimir.pf);
    fclose (fp);

    return(0);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
