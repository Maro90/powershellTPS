
#include <stdio.h>
#include <unistd.h>

int main (void)
{
	char c[32];
	printf("Presionar Enter para continuar con la ejecucion del progreso ... \n");

	if(fork () )
	{
		if(fork () )
		{
			printf("Inicia Padre \t%d\n", getpid() );
			fgets( c,32,stdin);
			printf("Fin	Padre\t%d\n", getpid() );
		}
		else
			if(fork() )
			{
				printf("Inicia Hijo2\t%d\n", getpid() );
				fgets( c,32,stdin);
				printf("Fin	Hijo2\t%d\n", getpid() );
			}
			else
			{
				printf("Inicia Bisnieto2\t%d\n", getpid() );
				fgets( c,32,stdin);
				printf("Fin	Bisnieto2\t%d\n", getpid() );
			}
	}
		else
			if(fork() )
			{ 
				printf("Inicia Hijo1 \t%d\n", getpid() );
				fgets( c,32,stdin);
				printf("Fin	Hijo1\t%d\n", getpid() );
			}
			else
			{
				printf("Inicia Bisnieto1\t%d\n", getpid() );
				fgets( c,32,stdin);
				printf("Fin	Bisnieto1\t%d\n", getpid() );
			}
	return 0;
}


/***********************************************************************************************
a) Se generan 5 procesos. el numero que imprime es el pid de cada proceso, es el idenficador unico que tiene el proceso.

c) Los procesos no necesariamente se crean y finalizan en el mismo orden, puede producirse un orden pero en otra ejecucion variar, esto se debe a que al crearse procesos diferentes estos pasan a competir por el procesador, y su finalizacion dependera del modo en que esta programado el planificador.

d) Desde otra terminar utilizamos el comando "ps -fea" observamos que estan los 5 procesos creados. Ademas utilizamos el comando "ps -aux" y observamos que los 5 procesos se encuentran en estado "S" lo cual significa que se encuentran suspendidos. Esto implica que los procesos estan esperando algun tipo de evento para volver al estado de Running nuevamente.
Al realizar el primer enter desaparece el primer proceso creado por el bash, al que llamamos padre, sus hijos,  cambian el ppid por el 1.
al presionar nuevamente "enter" desaparece el segundo proceso, y sucede lo mismo que antes, su proceso hijo toma el ppid 1.
al presionar nuevamente "enter" desaparece el tercer proceso, y sucede lo mismo que antes, su proceso hijo toma el ppid 1.
al presionar nuevamente "enter" desaparece el cuarto proceso.
al presionar nuevamente "enter" desaparece el quinto proceso.


*/








		
