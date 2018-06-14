#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ES_LETRA(x) ((x > 64 && x < 91) || (x > 96 && x < 123)) ? 1 : 0
#define ES_VOCAL(x) (tolower(x) == 97 || tolower(x) == 101 || tolower(x) == 105 || tolower(x) == 111 || tolower(x) == 118) ? 1 : 0
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct{
    char pathIn[100];
    char pathOut[100];
    char* nombreArchivo;
}t_dato;

typedef struct{
    FILE* pf;
    struct tm* tInfo1;
    int pid;
    struct tm* tInfo2;
    int cantVocales;
    int cantConsonantes;
    int cantCaracteres;
}t_imprimir;

// debería tener un array o algo donde voy guardando todos los archivos que manejo ese hilo
void    printfFiles(FILE*, struct tm*, int, struct tm*, int, int, int);
void*   analyze(char [], char [], char*);
int     countFiles();