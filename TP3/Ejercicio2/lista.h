#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
///////////////////////////////////////
typedef struct{
    char name[100];
    int pidThread;
}t_dato;

typedef struct s_nodo{
    t_dato dato;
    struct s_nodo *sig;
}t_nodo;

typedef t_nodo* t_lista;
typedef int (*t_cmp)(const void *e1,const void *e2);
typedef int (*t_act)(void *e1);
///////////////////////////////////////
void    crearLista(t_lista *p);
int     insertarOrdenado(t_lista *,const t_dato *,t_cmp cmp);
void    mostrarLista(t_lista *p);
void    vaciarLista(t_lista *p);
int     cmp(const void *e1,const void *e2);
int     eliminarPorPosicion(t_lista *p, int poss);
int     size(t_lista*);
void    cargarArchivosEnLista(t_lista *, char []);