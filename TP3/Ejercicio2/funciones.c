#include "parametros.h"
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

    fprintf(p.pf, "%d:%d:%d\n", p.tInfo1->tm_hour, p.tInfo1->tm_min, p.tInfo1->tm_sec);
    fprintf(p.pf, "PID: %d\n",p.pid);
    fprintf(p.pf ,"Vocales:%d\nConsonantes:%d\nCaracteres:%d\n", p.cantVocales, p.cantConsonantes, p.cantCaracteres);
    fprintf(p.pf, "%d:%d:%d\n", p.tInfo2->tm_hour, p.tInfo2->tm_min, p.tInfo2->tm_sec);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void imprimirPantalla(t_pantalla p){
    printf("Archivo con menor cantidad de caracteres:\t %d\t%s\n", p.minCaracter, p.minArchivo);
    printf("Archivo con mayor cantidad de caracteres:\t %ld\t%s\n", p.maxCaracter, p.maxArchivo);
    printf("Primer archivo procesado:\t\t\t\t%s\n", p.primerArchivo);
    printf("Último archivo procesado:\t\t\t\t%s\n", p.ultimoArchivo);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void comprobarMaxMin(t_pantalla* p, char n[], long int c){
    if(p->minCaracter > c){
        p->minCaracter = c;
        strcpy(p->minArchivo, n);
    }
    if(p->maxCaracter < c){
        p->maxCaracter = c;
        strcpy(p->maxArchivo, n);
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////