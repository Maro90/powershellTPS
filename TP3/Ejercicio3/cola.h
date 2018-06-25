#include <stdio.h>
#include <stdlib.h>

////////////////////////////////////////////////
typedef struct{
    int num;
}t_dato;

typedef struct s_nodo{
    t_dato dato;
    struct s_nodo *sig;
}t_nodo;

typedef struct{
    t_nodo *pri;
    t_nodo *ult;
}t_cola;
/////////////////////////////////////////////////////

void crearCola(t_cola *);
int aColar(t_cola *,const t_dato *);
int desAColar(t_cola *p,t_dato *);
int colaLlena(const t_cola *);
int colaVacia(const t_cola *);
void vaciarCola(t_cola *);
int verFrente(const t_cola *,t_dato *);
void cargarDato(t_dato *);
