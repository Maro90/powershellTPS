#!/bin/bash

#################################################
#			  Sistemas Operativos			 	#
#		Trabajo Práctico 2 - Ejericio 2			#
#		Nombre del Script: Ejercicio2.sh		#
#												#
#				Integrantes:					#
#		Arana, Juan Pablo 		33904497		#
#		Gonzalez, Mauro Daniel 	35368160		#
#		Sapaya, Nicolás Martín 	38319489		#
#												#
#		Instancia de Entrega: Entrega			#
#												#
#################################################

#	Funcion si se ingresaro parametros incorrectos.
ErrorSintaxOHelp() { 
	clear
	#	Si $1 es 1, entonces es error de sintaxis
	if test $1 != 0; then
		echo 'Error. La sintaxis del script es la siguiente:'
		echo 'Para ayuda:'
		echo "$0 -h"
		echo "$0 -?"
		echo "$0 -help"
		echo "----------------------------------"
	else
		echo 'Help:'
		echo "La sintaxis del script es la siguiente:"
	fi
	echo "$0 [Archivo][Archivo]"
	exit	
}

#	Es ayuda ?
if [ "$1" = "-h" ]; then
	ErrorSintaxOHelp 0
elif [ "$1" = "-help" ]; then
	ErrorSintaxOHelp 0
elif [ "$1" = "-?" ]; then
	ErrorSintaxOHelp 0
fi

if [ $# != 2 ]; then
    ErrorSintaxOHelp 1
    exit
fi
if ! [ -f "$1" ] || ! [ -f "$2" ]; then
    echo "Archivos invalidos"
    exit 
fi
file -i "$1" | grep text/plain >> /dev/null
if [ $? != 0 ]
then
    echo "El archivo esta vacio"
    exit 
fi
cp "$1" "$1.bak"

ARCH1=/tmp/`basename "$1"`.$$

cp "$1" "$ARCH1"

if [ -s "$2" ]; then
	PMAYUS=""
else
	PMAYUS=$(sed -r 's/(.*)/s\/\1\/\1\/I;/' "$2")
fi

REPLACE="
s/[A-Z]*/\L&/g
s/^./\u&/
s/([\.;,])\s*([a-z])/\1 \2/g
s/(\s+)/ /g
s/\.\s+$/./
$PMAYUS
"

sed -i -r "$REPLACE" "$ARCH1"

mv "$ARCH1" "$1"