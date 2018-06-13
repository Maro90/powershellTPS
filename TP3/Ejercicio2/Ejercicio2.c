#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>

#define ES_LETRA(x) ((x > 64 && x < 91) || (x > 96 && x < 123)) ? 1 : 0
#define ES_VOCAL(x) (tolower(x) == 97 || tolower(x) == 101 || tolower(x) == 105 || tolower(x) == 111 || tolower(x) == 118) ? 1 : 0

// debería tener un array o algo donde voy guardando todos los archivos que manejo ese hilo
void* analyze(char*);

int main(int argc, char const *argv[]){
    pthread_t hilo1;

    pthread_create(&hilo1, NULL, analyze("texto1.txt"), NULL);

    pthread_join(hilo1, NULL);

   //https://en.wikipedia.org/wiki/C_date_and_time_functions
   //http://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread_create.html
    //char a[30] = "Entrada/";
    //printf("%s\n",a);
    //printf("A=%s\n",strcat(a,"pepe"));
    //printf("%s\n",a);
    return 0;
}

int countFiles(){
    int file_count = 0;
    DIR * dirp;
    struct dirent * entry;

    dirp = opendir("Entrada/"); /* There should be error handling after this */
    
    if(!dirp){
        printf("No se pudo abrir el directorio.\n");
        exit(1);
    }

    entry = readdir(dirp);
    printf("%s\n", entry->d_name);
    
    while(!entry) {
        if(entry->d_type == DT_REG){ /* If the entry is a regular file */
            file_count++;
        }
        entry = readdir(dirp);
    }
    closedir(dirp);
}
void printFile(FILE* pf, struct tm* tInfo1, int pid, struct tm* tInfo2, int cVocales, int cConsonantes, int cCaracteres){
    fprintf(pf, "%d:%d:%d\n", tInfo1->tm_hour, tInfo1->tm_min, tInfo1->tm_sec);
    fprintf(pf, "PID: %d\n",pid);
    fprintf(pf ,"Vocales:%d\nConsonantes:%d\nCaracteres:%d\n", cVocales, cConsonantes, cCaracteres);
    fprintf(pf, "%d:%d:%d\n", tInfo2->tm_hour, tInfo2->tm_min, tInfo2->tm_sec);
}

void* analyze(char* name){
    printf("PID THREAD: %d\n",getpid());
    printf("FILES = %d\n",countFiles());
    FILE *fp;
    FILE *fout;
    char caracter;
    int countVocales = 0;
    int countConsonantes = 0;
    int countCaracteres = 0;
    char in[30] = "Entrada/";
    char out[30] = "Salida/";
    //////////////////////////////////////
    time_t timeBegin;
    time_t timeEnd;
    struct tm * timeInfoB;
    struct tm * timeInfoE;
    //////////////////////////////////////
    fp = fopen(strcat(in, name),"r");
    fout = fopen(strcat(out, name),"w");

    if(!fp){
        fputs("File IN error",stderr);
        exit(1);
    }
    if(!fout){
        fputs("File OUT error",stderr);
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

    // Cierro file pointers
    fclose (fout);
    fclose (fp);

    return(0);
}