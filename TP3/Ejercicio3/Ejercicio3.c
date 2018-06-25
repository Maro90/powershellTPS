#include "cola.h"

int main(){
    t_cola cola;
    t_dato dato;
    int i;
    crearCola(&cola);
    if(!colaLlena(&cola)){
        for(i=0;i<3;i++){
            cargarDato(&dato);
            aColar(&cola,&dato);
        }
    }
    if(!colaVacia(&cola)){
        desAColar(&cola,&dato);
        printf("Numero desacolado es = %d\n",dato.num);
    }
    if(!colaVacia(&cola)){
        verFrente(&cola,&dato);
        printf("El frente es = %d\n",dato.num);
    }
    vaciarCola(&cola);
    return 0;
}