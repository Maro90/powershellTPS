#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <sys/file.h>
#define _GNU_SOURCE

typedef struct pregunta{
    char* pregunta;
    char* respuesta1;
    char* respuesta2;
    char* respuesta3;
    char* respuesta4;
    int rc;
    struct pregunta * siguiente;
}t_pregunta;

t_pregunta *lista_preguntas;

void abrirArchivo();

int main(int argc, char *const argv[]){
    abrirArchivo();
    exit(EXIT_SUCCESS);
}

void abrirArchivo(){
    int cantidadDePreguntas = 0;
    int nroRespuesta = 0;

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    lista_preguntas = malloc(sizeof(t_pregunta));
    lista_preguntas->siguiente = NULL;

    fp = fopen("preguntas.txt", "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    t_pregunta *preguntas = lista_preguntas;

    while ((read = getline(&line, &len, fp)) != -1) {

        char type = line[0];
        if( type == 'P'){
            if(cantidadDePreguntas > 0){
                //Agrego nueva pregunta a la lista
                preguntas->siguiente = malloc(sizeof(t_pregunta));
                preguntas->siguiente->siguiente = NULL;
                preguntas = preguntas->siguiente;
            }
            
            cantidadDePreguntas++;
            nroRespuesta = 0;
            preguntas->pregunta = (char*)malloc(strlen(line + 2) * sizeof(char));
            strcpy(preguntas->pregunta,line + 2);
        } else {
            int offset = 2;
            if( line[1] == 'C'){    //Respuesta Correcta
                preguntas->rc = nroRespuesta;
                offset = 3;
            }

            switch(nroRespuesta){
                case 0:
                    preguntas->respuesta1 = (char*)malloc(strlen(line + offset) * sizeof(char));
                    strcpy(preguntas->respuesta1,line + offset);
                    break;
                case 1:
                    preguntas->respuesta2 = (char*)malloc(strlen(line + offset) * sizeof(char));
                    strcpy(preguntas->respuesta2,line + offset);
                    break;
                case 2:
                    preguntas->respuesta3 = (char*)malloc(strlen(line + offset) * sizeof(char));
                    strcpy(preguntas->respuesta3,line + offset);
                    break;
                case 3:
                    preguntas->respuesta4 = (char*)malloc(strlen(line + offset) * sizeof(char));
                    strcpy(preguntas->respuesta4,line + offset);
                    break;
            }
            nroRespuesta++;
        }
    }

    fclose(fp);
    if (line) {
        free(line);
    }
    printf("Cantidad de Preguntas: %d\n\n", cantidadDePreguntas);

    t_pregunta *pregunta = lista_preguntas;

    while(pregunta != NULL){
        printf("%s\n", pregunta->pregunta);
        printf("%s\n", pregunta->respuesta1);
        printf("%s\n", pregunta->respuesta2);
        printf("%s\n", pregunta->respuesta3);
        printf("%s\n", pregunta->respuesta4);
        printf("La correcta es la: %d\n\n", pregunta->rc + 1);
        pregunta = pregunta->siguiente;
    }
}
