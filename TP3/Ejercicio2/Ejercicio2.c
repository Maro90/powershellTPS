#include "parametros.h"

int main(int argc, char *const argv[]){
    int option;
    int multiplicidad;
    char pathIn[100];
    char pathOut[100];

    if(argc != 2 && argc != 4){
        printf("Ingrese ./Ejercicio2.exe h para ayuda.\n");
        exit(1);
    }else if(argc == 2 && (strcmp(argv[1],"h") == 0)){
        printf("La sintaxis es ./Ejercicio2.exe [pathEntrada] [pathSalida] [nivel paralelismo]\nFormato: [string] [string] [int]\n");
        exit(1);
    }else if((((int)*argv[3]) < 49) || (((int)*argv[3]) > 57)){ // compruebo que este entre 1 y 9
        printf("[nivel paralelismo] debe ser un número entre 1 y 9\n");
        exit(1);
    }

    // (((int)*argv[3])-48) es el parametro 3, que sería la multiplicidad
    strcpy(pathIn, argv[1]);
    strcpy(pathOut, argv[2]);
    multiplicidad = (((int)*argv[3])-48);
    
    // Empiezo a crear hilos
    pthread_t hilo1;

    pthread_create(&hilo1, NULL, analyze(pathIn, pathOut, "texto1.txt"), NULL);

    printf("join hilo\n");
    pthread_join(hilo1, NULL);
    printf("termino join\n");

    /* Declaring an array for 10 threads.
    //pthread_t thread_id [10];

    //Creating 10 threads with default attributes and the common function namely `functionA` for execution.
    for (int i = 0; i < 10; i++)
    {
        pthread_create (&thread_id [i], NULL, functionA, NULL);
    }*/
    return 0;
}

int countFiles(){
    int file_count = 0;
    DIR * dirp;
    struct dirent * entry;

    dirp = opendir("Entrada/");
    
    if(!dirp){
        printf("No se pudo abrir el directorio.\n");
        exit(1);
    }

    entry = readdir(dirp);

    while(entry != NULL) {
        if(entry->d_type == DT_REG){ // Si es archivo regular
            file_count++;
            printf("%s\n", entry->d_name);
        }
        entry = readdir(dirp);
    }
    closedir(dirp);
    return(file_count);
}

void printFile(FILE* pf, struct tm* tInfo1, int pid, struct tm* tInfo2, int cVocales, int cConsonantes, int cCaracteres){

    printf("Imprimiendo archivo\n");

    fprintf(pf, "%d:%d:%d\n", tInfo1->tm_hour, tInfo1->tm_min, tInfo1->tm_sec);
    fprintf(pf, "PID: %d\n",pid);
    fprintf(pf ,"Vocales:%d\nConsonantes:%d\nCaracteres:%d\n", cVocales, cConsonantes, cCaracteres);
    fprintf(pf, "%d:%d:%d\n", tInfo2->tm_hour, tInfo2->tm_min, tInfo2->tm_sec);
}

void* analyze(char pathIn[], char pathOut[], char* name){
    printf("PID THREAD: %d\n",getpid());
    printf("FILES = %d\n",countFiles());

    FILE *fp;
    FILE *fout;
    char caracter;
    int countVocales = 0;
    int countConsonantes = 0;
    int countCaracteres = 0;
    //////////////////////////////////////
    time_t timeBegin;
    time_t timeEnd;
    struct tm* timeInfoB;
    struct tm* timeInfoE;
    //////////////////////////////////////

    fp = fopen(strcat(pathIn, name),"r");
    fout = fopen(strcat(pathOut, name),"w");

    if(!fp){
        fputs("File IN error\n",stderr);
        exit(1);
    }
    if(!fout){
        fputs("File OUT error\n",stderr);
        exit(1);
    }

    // Tomo el tiempo de inicio
    time(&timeBegin);
    timeInfoB = localtime(&timeBegin);
    ///////////////////////////////////

    while((caracter = fgetc(fp)) != EOF){
        // printeo para ver el archivo esta al pedo
        printf("%c",caracter);
        // es letra?
        if(ES_LETRA(caracter)){
             // Es vocal o consontante?
            if(ES_VOCAL(caracter)){
                // Es vocal
                countVocales++;
            }else{
                // Es consonante
                countConsonantes++;
            }
        }else{
            // Si es /n no lo cuento
            if(caracter != 10){
                countCaracteres++;
            }
        }
	}
    printf("\n-------------------------------------------\n");
    printf("Vocales:%d\nConsonantes:%d\nCaracteres:%d\n", countVocales, countConsonantes, countCaracteres);

    // Tomo el tiempo de finalización
    time(&timeEnd);
    timeInfoE = localtime(&timeEnd);

    // Imprimo todo el file
    printFile(fout, timeInfoB, getpid(), timeInfoE, countVocales, countConsonantes, countCaracteres);

    printf("termine de imprimir\n");

    // Cierro file pointers
    fclose (fout);
    fclose (fp);
}