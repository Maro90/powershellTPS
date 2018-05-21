#!/bin/bash

#############################################
#    	    Sistemas Operativos		        #	
#     Trabajo Practico 2 - Ejercicio 5	    #
#		Integrantes:		                #
#      Arana, Juan Pablo        33904497    #
#      Gonzalez, Mauro Daniel   35368160    #
#      Sapaya, Nicolás Martín   38319489    #
#                                           #
#       Instancia de Entrega: Entrega       #
#				                            #
#############################################

#***********************************************************************************************

#Funcion si se ingreso parametros incorrectos.

ErrorSintaxOHelp() { 
	#clear
	#Si $1 es 1, entonces es error de sintaxis
	if test $1 != 0; then
		echo 'Error. La sintaxis del script es la siguiente:'
	else
		echo 'Help:'
		echo "La sintaxis del script es la siguiente:"
	fi
	
    echo "$0 [Archivo] -r"
	echo "$0 [Archivo] -r -p"
	echo "$0 [Archivo] -l [Legajo]"
	echo 'Para ayuda:'
	echo "$0 [-h]"
	echo "$0 [-?]"
	echo "$0 [--help]"
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

signal_SIGUSR1(){
    echo "Señal SIGUSR1"
}

signal_SIGUSR2(){
    echo "Señal SIGUSR2"
}

signal_SIGTERM(){
    echo "Señal SIGTERM"
    exit
}

#***********************************************************************************************
#***********************************************************************************************

#Se comprueba que haya solicitado ayuda
if test $1 == "-h" || test $1 == "--help" || test $1 == "-?"; then 
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
trap signal_SIGUSR1 SIGUSR1
trap signal_SIGUSR2 SIGUSR2

#Señal para terminar
trap signal_SIGTERM SIGTERM

#ignorando las siguientes señales
trap "" SIGHUP
trap "" SIGINT
trap "" SIGQUIT
trap "" SIGABRT
trap "" SIGALRM

echo "Mi pid es: $$" 
while true
do
  echo "Soy un bucle" >>  /dev/null
done
