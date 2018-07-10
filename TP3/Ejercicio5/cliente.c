#include "shared.h"
#include <string.h>

/*#################################################
#			  Sistemas Operativos			 	#
#		Trabajo Práctico 3 - Ejericio 5			#
#		Nombre del Script: cliente.c		    #
#												#
#				Integrantes:					#
#		Arana, Juan Pablo 		33904497		#
#		Gonzalez, Mauro Daniel 	35368160		#
#		Sapaya, Nicolás Martín 	38319489		#
#												#
#		Instancia de Entrega: 1er re Entrega	#
#												#
#################################################*/

void init(){
	for (int i = 0; i < 4; i++)
	{
		locks[i] = mutexGet(locksName[i]);
		if(locks[i]==NULL){
			printf("Server no encontrado\n");
			exit(1);
		}
	}

	message = getSharedMem(MESSAGE_NAME,sizeof(messageData),0);
	if(message == NULL){
		printf("Server no encontrado\n");
		exit(2);	
	}
}

void printHelp(int argc, char *const argv[]){
	if(argc == 2 && (strcmp(argv[1],"help")==0 || strcmp(argv[1],"-h")==0)){
		printf("Help:\n");
		printf("./cliente [texto a encriptar/desencriptar] [1-encriptar/2-desencriptar]\n");
		printf("---Ejemplo: ./cliente 'hola mundo' 1\n");
		printf("--->> #$'*k&>%%/$\n");
		printf("-> Si no se pasan argumentos se solicitaran por stdin\n");
		printf("-> Previamente el servidor debe estar ejecutando \n");
		exit(0);
	}
}

int main(int argc, char *const argv[]){
	printHelp(argc,argv);

	init();
	char messageInput[100];
	int type;

	if(argc >= 3){
		strcpy(messageInput,argv[1]);
		type=(int)*argv[2] - 48;
	}else{
		printf("Ingrese texto:");
		fgets(messageInput, 100, stdin);
		messageInput[strlen(messageInput) - 1]='\0';
		printf("1 - Encriptar, 2 Desencriptar:");
		fseek(stdin,0,SEEK_END);
		scanf(" %d",&type);
	}

	if(type<1 || type >2){
		printf("Operacion solicitada invalida");
		exit(1);
	}

	if(mutexLock(locks[CLIENTLOCK],TIMEOUT_DEFAULT)){
		printf("Servidor no responde\n");
		exit(1);
	}

	message->operation=type;
	strcpy(message->text,messageInput);

	mutexUnlock(locks[SERVERLOCK]);

	if(mutexLock(locks[CRIPTEDLOCK],TIMEOUT_DEFAULT)){
		printf("Servidor no responde\n");
		exit(1);
	}
	
	printf("%s", message->text);
	mutexUnlock(locks[DELETELOCK]);
	return 0;
}