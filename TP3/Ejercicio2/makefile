Ejercicio2: Ejercicio2.o funciones_Lista.o funciones.o
	gcc -o Ejercicio2 Ejercicio2.c funciones_Lista.c funciones.c -lpthread

Ejericicio2.o: Ejercicio2.c parametros.h
	gcc -c Ejercicio2.c

funciones_Lista.o: funciones_Lista.c lista.h
	gcc -c funciones_Lista.c

funciones.o: funciones.c parametros.h
	gcc -c funciones.c

clean:
	rm -f *.o