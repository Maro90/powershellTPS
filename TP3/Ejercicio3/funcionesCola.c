#include "cola.h"

/////////////////////////////////////////
void crearCola(t_cola *p){
    p->pri = NULL;
    p->ult = NULL;
}
/////////////////////////////////////////
int aColar(t_cola *p,const t_dato *d){
    t_nodo *nue;
    nue = (t_nodo*) malloc(sizeof(t_nodo));
    if(!nue){
        return 0;
    }
    nue->dato = *d;
    nue->sig = NULL;
    if(p->ult == NULL){
        p->pri = nue;
    }else{
        p->ult->sig = nue;
    }
    p->ult = nue;
    return 1;
}
/////////////////////////////////////////
int desAColar(t_cola *p,t_dato *d){
    t_nodo *aux;
    if(p->pri == NULL){
        return 0;
    }
    *d = p->pri->dato;
    aux = p->pri;
    p->pri = p->pri->sig;
    free(aux);
    return 1;
}
/////////////////////////////////////////
int colaLlena(const t_cola *p){
    void *aux;
    aux = malloc(sizeof(t_nodo));
    free(aux);
    return aux == NULL;
}
/////////////////////////////////////////
int colaVacia(const t_cola *p){
    return p->ult == NULL;
}
/////////////////////////////////////////
void vaciarCola(t_cola *p){
    t_nodo *aux;
    while(p->pri != NULL){
        aux = p->pri;
        p->pri = p->pri->sig;
        free(aux);
    }
}
/////////////////////////////////////////
int verFrente(const t_cola *p,t_dato *d){
    if(p->pri == NULL)
    {
        return 0;
    }
    *d = p->pri->dato;
    return 1;
}
/////////////////////////////////////////
void cargarDato(t_dato *d){
    puts("Ingrese numero");
    scanf("%d",&(d->num));
 }
