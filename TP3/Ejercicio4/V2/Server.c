#include<stdlib.h> 
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h> 
#include <stdio.h>

#include "Game.h"


int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
     
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("No se pudo abrir el socket");
        exit(1);
    }
     
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("No se pudo abrir el socket");
        exit(1);
    }

    listen(socket_desc , 3);
     
    printf("Esperando conexiones...\n");
    c = sizeof(struct sockaddr_in);
	

    while( 1 ){
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        printf("Nueva conexion\n");
        if (client_sock < 0){
            printf("No se pudo aceptar conexion\n");
            continue;
        }

        newUser(client_sock);
    }
     

    return 0;
}
