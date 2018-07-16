#include "Protocol.h"
#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

void acceptController(tMessageAccept msg, tConnection * connection){
   connection->id = msg.id;
   printf("%s\n",msg.message);
}

void questionController(tMessageQuestion msg, tConnection * connection){
   printf("--> %s\n",msg.title); //deberia mostrar la pregunta y las posibles respuestas (ver struct tMessageQuestion en protocol )
   printf("--> %s\n",msg.answers[0]); //deberia mostrar la pregunta y las posibles respuestas (ver struct tMessageQuestion en protocol )
   printf("--> %s\n",msg.answers[1]); //deberia mostrar la pregunta y las posibles respuestas (ver struct tMessageQuestion en protocol )
   printf("--> %s\n",msg.answers[2]); //deberia mostrar la pregunta y las posibles respuestas (ver struct tMessageQuestion en protocol )
   printf("--> %s\n",msg.answers[3]); //deberia mostrar la pregunta y las posibles respuestas (ver struct tMessageQuestion en protocol )
   


   tCommand cmd;
   cmd.commandId = COMMAND_ANSWER;
   tMessageAnswer msg_ans;
   msg_ans.id = 3; // aca deberia completar el user con scanf
   send_command(&cmd,&msg_ans,connection);
}

void quitController(tMessageQuit msg, tConnection * connection){
   printf("%s \n",msg.message);
   exit(0);
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