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
    char* respuestas[4];
    int rc;
    struct pregunta * siguiente;
}t_pregunta;

t_pregunta *lista_preguntas = NULL;

void abrirArchivoDePreguntas();
void agregarPreguntaALista(char * pregunta, char * respuestas[], int respuestaCorrecta);

int main(int argc, char *const argv[]) {
    abrirArchivoDePreguntas();
    exit(EXIT_SUCCESS);
}

//---------------------------------------------------------------------------------------------------
/*
*   Se encarga de leer el archivo de preguntas y cargar la lista.
*
*/
void abrirArchivoDePreguntas() {
    int cantidadDePreguntas = 0;
    int nroRespuesta = 0;

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("preguntas.txt", "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
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
                agregarPreguntaALista(preg,resp,respuestaCorrecta);
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
    printf("Cantidad de Preguntas: %d\n\n", cantidadDePreguntas);

    t_pregunta *punteroLista = lista_preguntas;

    int pregu = 1;
    while(punteroLista != NULL){
            printf("Pregunta %d: %s",pregu, punteroLista->pregunta);
            for(int i=0; i<4;i++){
                printf("Respuesta %d: %s",i+1, punteroLista->respuestas[i]);
            }
            printf("\n");
            pregu++;
            punteroLista = punteroLista->siguiente;
    }
}


//---------------------------------------------------------------------------------------------------
/*
*   Se encarga de agregar las preguntas a la lista.
*
*/
void agregarPreguntaALista(char * pregunta, char * respuestas[], int respuestaCorrecta) {
    t_pregunta *punteroLista = lista_preguntas;
    
    if (punteroLista == NULL) { //Primer Pregunta, lista vacía.
        
        //Inicializo la lista
        punteroLista = malloc(sizeof(t_pregunta));
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
        punteroLista->siguiente = NULL;
        ultimaPregunta->siguiente = punteroLista;
    }

    //Cargo la pregunta
    punteroLista->pregunta = (char*)malloc(strlen(pregunta) * sizeof(char));
    strcpy(punteroLista->pregunta,pregunta);
    
    punteroLista->rc = respuestaCorrecta;
    
    //Cargo las respuestas
    for(int i=0; i<4;i++) {
        punteroLista->respuestas[i] = (char*)malloc(strlen(respuestas[i]) * sizeof(char));
        strcpy(punteroLista->respuestas[i],respuestas[i]);
    }
}



