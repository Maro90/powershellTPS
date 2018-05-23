#!/bin/bash

#################################################
#			  Sistemas Operativos			 	#
#		Trabajo Práctico 2 - Ejericio 6			#
#		Nombre del Script: Ejercicio3.sh		#
#												#
#				Integrantes:					#
#		Arana, Juan Pablo 		33904497		#
#		Gonzalez, Mauro Daniel 	35368160		#
#		Sapaya, Nicolás Martín 	38319489		#
#												#
#		Instancia de Entrega: Entrega			#
#												#
#################################################

FOLDER_NAME=""
FILECOUNT=0
SIZE=0
IGNORE=0

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
	else
		echo 'Help:'
		echo "La sintaxis del script es la siguiente:"
	fi
	echo "$0 [Directorio]"
	exit	
}

function parseLine(){
	LINE=$1
	ISNEWDIR=$(echo $LINE | grep -c ":$")
	if [[ $ISNEWDIR > 0 ]]; then
		if [[ $IGNORE == 0 ]] && [[ $FOLDER_NAME != "" ]];then
			print	
		fi
		FOLDER_NAME=$LINE
		FILECOUNT=0
		SIZE=0
		IGNORE=0
	else
		ISDIR=$(echo $LINE | grep -c "^d")
		if [[ $ISDIR > 0 ]]; then
			IGNORE=1
		else 
			ISFILE=$(echo $LINE | grep -c "^-")
			if [[ $ISFILE > 0 ]]; then
				FSIZE=$(echo $LINE | grep -o  "^[rwdx-]\+\s\+[0-9]\+\s[a-zA-Z0-9]\+\s\+[a-zA-Z0-9]\+\s\+[0-9]\+" | grep -o "[0-9]\+$") 
				let SIZE=SIZE+FSIZE
				let FILECOUNT++
			fi
		fi
	fi
}

function print(){
	let SIZE=SIZE/1000
	echo $FOLDER_NAME"	"$FILECOUNT"	"$SIZE
}

#	Se comprueba que la cantidad de parametros este entre 1 y 3
if (test $# -ne 1); then 	
	ErrorSintaxOHelp 1
fi

#	Es ayuda ?
if [ "$1" = "-h" ]; then
	ErrorSintaxOHelp 0
elif [ "$1" = "-help" ]; then
	ErrorSintaxOHelp 0
elif [ "$1" = "-?" ]; then
	ErrorSintaxOHelp 0
fi

echo "FOLDER		COUNT		SIZE[KB]"
ls -LRl $1 | ( while read -r line; do parseLine "$line"; done; print) | sort -k3 --numeric-sort | head -n 10