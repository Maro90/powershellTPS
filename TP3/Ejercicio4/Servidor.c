#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <sys/file.h>
#define _GNU_SOURCE

typedef struct{
    char* pregunta;
    char* respuesta1;
    char* respuesta2;
    char* respuesta3;
    char* respuesta4;
    int rc;
}t_pregunta;

t_pregunta preguntas[1];

void abrirArchivo();

int main(int argc, char *const argv[]){
    abrirArchivo();
    exit(EXIT_SUCCESS);
}

void abrirArchivo(){
    int cantidadDePreguntas = -1;
    int nroRespuesta = 0;

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("preguntas.txt", "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }
    while ((read = getline(&line, &len, fp)) != -1) {
        char type = line[0];
        if( type == 'P'){
            cantidadDePreguntas++;
            nroRespuesta = 0;
            strcpy(preguntas[cantidadDePreguntas].pregunta,line + 2);
        } else {
            switch(nroRespuesta){
                case 0:
                    strcpy(preguntas[cantidadDePreguntas].respuesta1,line + 2);
                    break;
                case 1:
                    strcpy(preguntas[cantidadDePreguntas].respuesta2,line + 2);
                    break;
                case 2:
                    strcpy(preguntas[cantidadDePreguntas].respuesta3,line + 2);
                    break;
                case 3:
                    strcpy(preguntas[cantidadDePreguntas].respuesta4,line + 2);
                    break;
            }
            if( line[1] == 'C'){    //Respuesta Correcta
                preguntas[cantidadDePreguntas].rc = nroRespuesta;
            }
            nroRespuesta++;
        }
    }

    fclose(fp);
    if (line) {
        free(line);
    }

    for(int i = 0; i<cantidadDePreguntas;i++){
            printf("%s\n", preguntas[i].pregunta);
    }
}
