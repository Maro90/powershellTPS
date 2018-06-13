#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

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

void* analyze(char* name){
    printf("%d",getpid());
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

    // Tomo el tiempo de inicio y lo imprimo en el file
    time(&timeBegin);
    timeInfoB = localtime(&timeBegin);

    fprintf(fout, "%d:%d:%d\n", timeInfoB->tm_hour, timeInfoB->tm_min, timeInfoB->tm_sec);
    ///////////////////////////////////

    while((caracter = fgetc(fp)) != EOF){
        // printeo para ver el archivo esta al pedo
        printf("%c",caracter);
        // es letra?
        if(ES_LETRA(caracter)){
        //if((caracter > 64 && caracter < 91) || (caracter > 96 && caracter < 123)){
             // Es vocal o consontante?
            if(ES_VOCAL(caracter)){
            //if(tolower(caracter) == 97 || tolower(caracter) == 101 || tolower(caracter) == 105 || tolower(caracter) == 111 || tolower(caracter) == 118){
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

    // Tomo el tiempo de finalización y lo imprimo en el file
    time(&timeEnd);
    timeInfoE = localtime(&timeEnd);

    fprintf(fout ,"Vocales:%d\nConsonantes:%d\nCaracteres:%d\n", countVocales, countConsonantes, countCaracteres);
    fprintf(fout, "%d:%d:%d\n", timeInfoE->tm_hour, timeInfoE->tm_min, timeInfoE->tm_sec);

    // Cierro file pointers
    fclose (fout);
    fclose (fp);
}