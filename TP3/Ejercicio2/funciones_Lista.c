#include "lista.h"
/////////////////////////////////////////
void crearLista(t_lista *p){
    *p = NULL;
}
//////////////////////////////////////////////////////////////////////////////////
int insertarOrdenado(t_lista *p,const t_dato *d,t_cmp cmp){
    t_nodo *nue;
    while(*p && (cmp(&((*p)->dato),d)<0)){
        p= &(*p)->sig;
    }
    if(*p && (cmp(&((*p)->dato),d)==0)){
        return 0;
    }
    nue = (t_nodo*) malloc(sizeof(t_nodo));
    if(!nue){
        return 0;
    }
    nue->sig=*p;
    nue->dato= *d;
    *p=nue;
    return 1;
 }
//////////////////////////////////////////////////////////////////////////////////
 int cmp(const void *e1,const void *e2){
    //e1 es la clave de la lista
    //e2 es la clave de la nueva info
    return strcmp(((t_dato*)e1)->name,((t_dato*)e2)->name);
}
//////////////////////////////////////////////////////////////////////////////////
void vaciarLista(t_lista *p){
    t_nodo *aux;
    while(*p){
        aux = *p;
        p = &(*p)->sig;
        free(aux);
    }
}
//////////////////////////////////////////////////////////////////////////////////
void mostrarLista(t_lista *p){
    printf("***********************************************\n");
    printf("Contenido de la lista:\n");
    while(*p){
        printf("Pid: %d - Name: %s\n",((*p)->dato).pidThread,((*p)->dato).name);
        p = &(*p)->sig;
    }
    printf("***********************************************\n");
}
//////////////////////////////////////////////////////////////////////////////////
int eliminarPorPosicion(t_lista *p, int poss){
    t_nodo *aux;
    int cont = 1;
    if(poss<=0){
        return 0;
    }
    while((cont<poss) && (*p)){
        p = &(*p)->sig;
        cont++;
    }
    aux = *p;
    *p = aux->sig;
    free(aux);
    return 1;
}
//////////////////////////////////////////////////////////////////////////////////
int size(t_lista *p){
    int size = 0;
    while(*p){
        size++;
        p = &(*p)->sig;
    }
    return size;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void cargarArchivosEnLista(t_lista* p, char pIn[]){
    t_dato d;
    DIR * dirp;
    struct dirent * entry;

    dirp = opendir(pIn);

    if(!dirp){
        printf("No se pudo abrir el directorio.\n");
        exit(1);
    }

    entry = readdir(dirp);

    while(entry != NULL) {
        if(entry->d_type == DT_REG){ // Si es archivo regular
            strcpy(d.name, entry->d_name);
            d.pidThread = 0;
            insertarOrdenado(p,&d,cmp);
        }
        entry = readdir(dirp);
    }
    closedir(dirp);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//ARREGLAR EL COMPARADOR DE INSERTAR ORDENADO, QUE INSERTE IGUAL SI TIENEN EL MISMO PID
// Y QUE PONGA PRIMERO EL TEXTO POR ABECEDARIO
