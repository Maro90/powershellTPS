#include "shared.h"
#include <string.h>

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

int main(int argc, char *const argv[]){
	init();
	char messageInput[100];
	int type;

	if(argc >= 3){
		strcpy(messageInput,argv[1]);
		type=(int)*argv[2] - 48;
	}else{
		printf("Ingrese texto:");
		scanf("%s",messageInput);
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