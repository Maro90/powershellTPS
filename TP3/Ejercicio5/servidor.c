#include "shared.h"
#include <signal.h>
#include <string.h>

/*#################################################
#			  Sistemas Operativos			 	#
#		Trabajo Práctico 3 - Ejericio 5			#
#		Nombre del Script: servidor.c		    #
#												#
#				Integrantes:					#
#		Arana, Juan Pablo 		33904497		#
#		Gonzalez, Mauro Daniel 	35368160		#
#		Sapaya, Nicolás Martín 	38319489		#
#												#
#		Instancia de Entrega: Entrega			#
#												#
#################################################*/

void terminateServer(){
	printf("\nLimpiando memoria compartida\n");
	if (message != NULL){
		removeSharedMem(MESSAGE_NAME,message,sizeof(messageData));
	}
	for(int i=0;i<4;i++){
		if(locks[i]!=NULL){
			removeSharedMem(locksName[i],locks[i],sizeof(mutex));
		}
	}
	printf("-FIN DEL PROGRAMA-\n");
	exit(0);
}

void sigInt(int dummy){
	terminateServer();
}

void init(){
	for(int i=0;i<4;i++){
		locks[i]=NULL;
	}

	printf("Iniciando server...\n");

	locks[SERVERLOCK] = mutexCreate(locksName[SERVERLOCK]);
	if (mutexLock(locks[SERVERLOCK],TIMEOUT_DEFAULT)){
		printf("No se pudo iniciar el servidor - error al lockear server [SERVERLOCK]\n");
		terminateServer();
		return;
	}

	locks[CRIPTEDLOCK] = mutexCreate(locksName[CRIPTEDLOCK]);
	if (mutexLock(locks[CRIPTEDLOCK],TIMEOUT_DEFAULT)){
		printf("No se pudo iniciar el servidor - error al lockear server [CRIPTEDLOCK]\n");
		terminateServer();
		return;
	}

	locks[DELETELOCK] = mutexCreate(locksName[DELETELOCK]);
	if (mutexLock(locks[DELETELOCK],TIMEOUT_DEFAULT)){
		printf("No se pudo iniciar el servidor - error al lockear server [DELETELOCK]\n");
		terminateServer();
		return;
	}


	message = getSharedMem(MESSAGE_NAME,sizeof(messageData),TIMEOUT_DEFAULT);
	if (message == NULL){
		printf("No se pudo iniciar el servidor - error al crear memoria de mensaje\n");

		terminateServer();
		return;
	}

	locks[CLIENTLOCK] = mutexCreate(locksName[CLIENTLOCK]);

	for(int i=0;i<4;i++){
		if(locks[i]==NULL){
			printf("No se pudo iniciar el servidor - lock no creado %s\n",locksName[i]);
			terminateServer();
			return;
		}
	}

	printf("Server iniciado ok\n");
}

void encryptDecrypt(char m[]){
	for(int i=0; i< strlen(m);i++){
		m[i] = m[i] ^ 'K';
	}
}

void processMessage(){
	printf("Ingreso message: %s operation: %d\n",message->text,message->operation);
	message->operation=0;
	encryptDecrypt(message->text);
	mutexUnlock(locks[CRIPTEDLOCK]);
	if(mutexLock(locks[DELETELOCK],TIMEOUT_DEFAULT)){
		printf("Cliente no respondio a tiempo.\n");
		mutexLock(locks[CRIPTEDLOCK],1);
	}
	strcpy(message->text,"");
	printf("Limpiando memoria\n");
	mutexUnlock(locks[CLIENTLOCK]);
	printf("Realizado!\n");
}

void printHelp(int argc, char *const argv[]){
	if(argc == 2 && (strcmp(argv[1],"help")==0 || strcmp(argv[1],"-h")==0)){
		printf("Help:\n");
		printf("./servidor\n");
		printf("-> No requiere argumentos\n");
		printf("-> Se mostrara por stdout los logs\n");
		printf("-> ctrl + c (sigint) para salir\n");
		exit(0);
	}
}

int main(int argc, char *const argv[]){
	printHelp(argc,argv);

	signal(SIGINT, sigInt);
	init();
	printf("Esperando mensajes..");
	while(1){
		if(mutexLock(locks[SERVERLOCK],TIMEOUT_DEFAULT)){
			printf(".");
			fflush(stdout);
			continue;
		}
		printf("\n");
		processMessage();
		printf("\nEsperando mensajes..");
	}

	terminateServer();
	return 0;
}