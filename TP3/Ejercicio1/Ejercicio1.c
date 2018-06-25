
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

char *tipo(int tipo_id){
	switch(tipo_id){
		case 1:
			return "Hijo";
		case 2:
			return "Nieto";
		case 3:
			return "Bisnieto";
		case -1:
			return "Zombie";
		case -2:
			return "Demonio";
	}
	return "Desconocido";
}

void mostar(pid_t pid_padre,int gen){
	//printf("Padre: %d\tPID: %d\t GEN:%d\n", pid_padre, getpid(),gen);
	printf("Soy el proceso con PID %d y pertenezco a la generación Nº %d Pid padre: %d Parentesco/Tipo: %s\n", getpid(),gen,pid_padre,tipo(gen));
}

void crear_zombies(int cant){
	pid_t pid_padre = getpid();
	
	for(int i=0;i<cant;i++){
		pid_t zombie = fork();	
		if(!zombie){
			mostar(pid_padre,-1);
			exit(0);
		}
	}
}

void crear_demonios(int cant){
	pid_t pid_padre = getpid();
	
	for(int i=0;i<cant;i++){
		pid_t demonio = fork();	
		if(!demonio){
			mostar(pid_padre,-2);
			setsid();
		}
	}
}

void crear_hijos(int gen, int gen_max ,int hijos_generacion [],int hijos_limite []){
	pid_t pid_padre = getpid();

	if(hijos_generacion[gen]>0 && hijos_limite[gen]>0){
		hijos_generacion[gen]--;
		hijos_limite[gen]--;

		pid_t hijo = fork();
		if(!hijo){ // es hijo
			gen++;
			mostar(pid_padre,gen);
		}else{
			int usage=1;
			for(int i=1;gen+i<gen_max;i++){
				usage *= hijos_limite[gen+i];
				hijos_generacion[gen+i]-= usage;
			}
		}

		if(gen<gen_max){
			crear_hijos(gen, gen_max ,hijos_generacion, hijos_limite);
		}else if(gen ==gen_max && hijos_generacion[gen_max-1]==0){
			crear_zombies(hijos_generacion[gen_max]);
			crear_demonios(hijos_generacion[gen_max+1]);
		}		
	}
}

void printHelp(int argc, char *const argv[]){
	if(argc == 2 && (strcmp(argv[1],"help")==0 || strcmp(argv[1],"-h")==0)){
		printf("Help:\n");
		printf("./Ejercicio1\n");
		printf("-> No requieren argumentos\n");
		printf("-> Presionar cualquier tecla para terminar los procesos\n");
		exit(0);
	}
}

int main (void)
{
	
	printf("Presionar Enter para continuar con la ejecucion del progreso ... \n");

	// ultimas dos generaciones {zombies, demonios}
	int hijos_generacion [5] = { 4, 7, 3, 5, 2};
	int hijos_limite[3] = {4, 2, 2};


	crear_hijos(0, 3, hijos_generacion,hijos_limite);

	char c[32];
	fgets( c,32,stdin);

	return 0;
}







		
