#!/bin/bash

#################################################
#			  Sistemas Operativos			 	#
#		Trabajo Práctico 2 - Ejericio 5			#
#		Nombre del Script: Ejercicio5.sh		#
#												#
#				Integrantes:					#
#		Arana, Juan Pablo 		33904497		#
#		Gonzalez, Mauro Daniel 	35368160		#
#		Sapaya, Nicolás Martín 	38319489		#
#												#
#		Instancia de Entrega: Entrega			#
#												#
#################################################

#***********************************************************************************************

#Funcion si se ingreso parametros incorrectos.

ErrorSintaxOHelp() { 
	clear
	#Si $1 es 1, entonces es error de sintaxis
	if test $1 != 0; then
		echo 'Error. La sintaxis del script es la siguiente:'
        echo 'Para ayuda:'
        echo "$0 [-h]"
        echo "$0 [-?]"
        echo "$0 [--help]"
        echo "--------------------------------------"
	else
		echo 'Help:'
		echo "La sintaxis del script es la siguiente:"
	fi
	
    echo "$0 [Archivo .log]"
	exit	
}

#***********************************************************************************************

#Funcion si el archivo de registros no existe, no tiene permisos de lectura o esta vacío.

ErrorVacioInex() { 
	clear
	if [ ! -e $1 ]; then
        echo "'$1' no existe."
	elif [ ! -r $1 ]; then
		echo "No tiene permisos de lectura para '$1'."
	elif [ ! -s $1 ]; then
		echo "'$1' es vacio."
	fi
	exit	
}


#***********************************************************************************************
#***********************************************************************************************

#Se comprueba que haya solicitado ayuda
if test "$1" = "-h" || test "$1" = "--help" || test "$1" = "-?"; then 
	#Es -h -help o -?
	ErrorSintaxOHelp 0
fi

#Si no es ayuda, se comprueba que la cantidad de parámetros sea válida
#Se comprueba que la cantidad de parametros este entre 1 y 3
if (test $# != 1); then 	
	ErrorSintaxOHelp 1
fi

#Capturo las señales SIGUSR1 y SIGUSR2
#   
#   Para enviarselas al proceso   "kill -s SIGUSR1 [PID]  -> se busca con  ps "
#

#Se valida que el demonio pueda correrse, es decir que se tengan permisos de ejecución.
if !(test -x "./demonio.sh"); then
	echo "Debe darle permisos de ejecución al demonio"
	exit
fi

process=$(ps -ef) #Cambio para que funcione coreectamente en ubuntu
if echo $process | grep -q "./demonio.sh"; then
  	echo "Ya hay corriendo un demonio de este proceso. Intentalo despues que finalice.";
else

	if [ ! -e "$1" ]; then	#no existe el archivo
		touch "$1"
	fi
	nohup ./demonio.sh $1 > /dev/null 2>&1 & echo "Demonio con el Pid: $!"
fi

