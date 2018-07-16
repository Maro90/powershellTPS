#include<stdlib.h> 
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h> 
#include <stdio.h>
#include <signal.h>

#include "Game.h"

int socket_desc;

void sigInt(int dummy){
    close(socket_desc);
    endGame();
    socket_desc=0;
}

int main(int argc , char *argv[])
{
    int client_sock , c;
    struct sockaddr_in server , client;

    signal(SIGINT, sigInt);

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
	
    configGame();

    while( socket_desc ){
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
