#include "Protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

pthread_t   threadResponder;

void * mostrarPregunta(tMessageQuestion * msg){
    printf("\n---------------------------------------------------------------------\n");
	printf("%s\n",msg->title);

    for(int i=0; i<4; i++){
        printf("RTA %d: %s\n", i+1, msg->answers[i]);
    }
    int respuesta;
    int ch;
    int ok;
    do{
        printf("Elija su respuesta: ");
        fflush(stdout);
        ok = scanf("%d", &respuesta);   
        /* Descartamos el resto de la linea */
        while ((ch = getchar()) != EOF && ch != '\n');
    }while (!ok || respuesta < 1 || respuesta > 4);

    respuesta = respuesta - 1; //La transformamos en el id
    pthread_exit((void*)respuesta);
}

void acceptController(tMessageAccept msg, tConnection * connection){
   connection->id = msg.id;
   printf("%s\n",msg.message);
}

void questionController(tMessageQuestion msg, tConnection * connection){
    int respuesta = -1;
    void *res;    
    pthread_create(&threadResponder, NULL, (void*)mostrarPregunta, (void*)&msg);

	pthread_join(threadResponder, &res);

    if( res == PTHREAD_CANCELED){
        printf("Tiempo agotado\n");
    } else {
        respuesta = res;
    }

    tCommand cmd;
    cmd.commandId = COMMAND_ANSWER;
    tMessageAnswer msg_ans;
    msg_ans.id = respuesta;
    send_command(&cmd,&msg_ans,connection);
}

void quitController(tMessageQuit msg, tConnection * connection){
   printf("%s \n",msg.message);
   exit(0);
}

void statisticsController(tMessageQuit msg, tConnection * connection){
   printf("%s\n",msg.message);

   pthread_cancel(threadResponder);

}

int main(int argc, char *argv[]) {
   int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;
	char messageInput[100];

   portno = 8888;
   
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
	
   server = gethostbyname("localhost");
   
   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);
   
   /* Now connect to the server */
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR connecting");
      exit(1);
   }
   

   printf("Ingrese usuario:");
   fgets(messageInput, 100, stdin);
   messageInput[strlen(messageInput) - 1]='\0';
   //validar user

   tConnection connection;
   connection.descriptor = sockfd;
   connection.acceptController = acceptController;
   connection.quitController = quitController;
   connection.questionController = questionController;

   tCommand cmd;
   cmd.commandId = COMMAND_INIT;
   
   tMessageInit msg;
   strcpy(msg.user,messageInput);

   send_command(&cmd,&msg,&connection);
  
   handler_run((void *)&connection);

   return 0;
}