#!/bin/bash

#################################################
#			  Sistemas Operativos			 	#
#		Trabajo Práctico 2 - Ejericio 3			#
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

#	Funcion si se ingresaro parametros incorrectos.
ErrorSintaxOHelp() { 
	clear
	#	Si $1 es 1, entonces es error de sintaxis
	if test $1 != 0; then
		echo 'Error. La sintaxis del script es la siguiente:'
	else
		echo 'Help:'
		echo "La sintaxis del script es la siguiente:"
	fi
	echo "$0 [Archivo][Archivo]"	
	echo "$0 [Archivo][Archivo][-i]"
	echo 'Para ayuda:'
	echo "$0 -h"
	echo "$0 -?"
	echo "$0 -help"
	exit	
}

#	Funcion si los archivos son vacios o inexistentes.
ErrorVacioInex() { 
	clear
	if [ -e $1 ]; then
		if [ ! -s $1 ]; then
			echo "$1 es vacio."
		fi
	else
		echo "$1 no existe."
	fi

	if [ -e $2 ]; then
		if [ ! -s $2 ]; then
			echo "$2 es vacio."
		fi
	else
		echo "$2 no existe."
	fi
	exit	
}

#	Se comprueba que la cantidad de parametros este entre 1 y 3
if (test $# -lt 1 || test $# -gt 3); then 	
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

if test $# -gt 1; then
	#	No es ayuda, entonces es archivo
	if test -e "$PWD/$1"; then
	 	if test -s "$PWD/$1"; then
	 		#	Los archivos existen y los asigno a una variable
	 		archivoPalabras="$PWD/$1"
	 	else
	 		#	Archivo Inexistente o Vacio
			ErrorVacioInex "$PWD/$1" "$PWD/$2"
		fi

		if test -e "$PWD/$2"; then
			if test -s "$PWD/$2"; then
				#	Los archivos existen y los asigno a una variable
				archivoBusco="$PWD/$2"
			else
				#	Archivo Inexistente o Vacio
				ErrorVacioInex "$PWD/$1" "$PWD/$2"
			fi
		else
	 		#	Archivo Inexistente o Vacio
			ErrorVacioInex "$PWD/$1" "$PWD/$2"
		fi

		#	Si son 3 parametros pregunto por el -i
		if test $# -ne 2; then
			#	Me fijo si ingreso -i para case sensitive
			if test $3 = "-i"; then
				sensitive=$3
			else
				ErrorSintaxOHelp 1
			fi
		fi
	else
		#	Archivo Inexistente o Vacio
		ErrorVacioInex "$PWD/$1" "$PWD/$2"
	fi
else
	#	Es ayuda
	ErrorSintaxOHelp 1
fi

let count=0

for line in $(cat "$archivoPalabras"); do
	wordsA[$count]=$line
    ((count++))
done

#	Inicializo count1 para iterar
#	noWords tiene la cantidad total de palabras en archivob.txt
let count1=0

noWords=$(cat "$archivoBusco" | wc -w)

#	Itero para contar las coincidencias
#	Tambien cuento las palabras que no estan en archivoA
for item in ${wordsA[*]}
do
	#   -o print only matched (non-empty) partos of matching lines
	#	wc -w cuenta las palabras
	if test "$sensitive" = "-i"; then
		ocurrencias[$count1]=$(tr '[:upper:]' '[:lower:]' < "$archivoBusco" | grep -o $item $_ | wc -w)
	else
		ocurrencias[$count1]=$(grep -o $item "$archivoBusco" | wc -w)
	fi
	let noWords=noWords-ocurrencias[$count1]
	((count1++))	
done

#	maxCant = cantidad palabras en parametro 1
maxCant=${#wordsA[*]}
i=0

#	Imprimo el resultado final
while [ $i -ne $maxCant ]
do
	echo ${wordsA[$i]}':' ${ocurrencias[$i]}
	((i++))
done

echo "No existen en $1:" $noWords