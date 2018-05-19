#!/bin/bash

#borra archivo
#rm ./a.txt
#convierte MAYUS a MINUS
#tr '[:upper:]' '[:lower:]' < ./archivob.txt

#Funcion si se ingresaro parametros incorrectos.
ErrorSintaxOHelp() { 
	clear

	#Si $1 es 1, entonces es error de sintaxis
	if test $1 != 0; then
		echo 'Error. La sintaxis del script es la siguiente:'
	else
		echo 'Help:'
		echo "La sintaxis del script es la siguiente:"
	fi

	echo "$0 [Archivo][Archivo]"
	
	echo "$0 [Archivo][Archivo][-i]"

	echo 'Para ayuda:'

	echo "$0 [-h]"
	
	echo "$0 [-?]"

	echo "$0 [-help]"
	
	exit	
}

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

#Se comprueba que la cantidad de parametros este entre 1 y 3
if (test $# -lt 1 || test $# -gt 3); then 	
	ErrorSintaxOHelp 1
fi

if test $1 != "-h" && test $1 != "-help" && test $1 != "-?"; then 
	#No es ayuda, entonces es archivo
	if test -e $1 && test -s $1 && test -e $2 && test -s $2; then
		#Los archivos existen y los asigno a una variable
		archivoPalabras=$1
		archivoBusco=$2
		#Si son 3 parametros pregunto por el -i
		if test $# -ne 2; then
			#Me fijo si ingreso -i para case sensitive
			if test $3 = "-i"; then
				echo 'No es Case Sensitive'
				sensitive=$3
			else
				ErrorSintaxOHelp 1
			fi
		fi
	else
		#Archivo Inexistente o Vacio
		ErrorVacioInex $1 $2
	fi
else
	#Es -h -help o -?
	ErrorSintaxOHelp 0
fi

# Link filedescriptor 10 with stdin
exec 10<&0
# stdin replaced with a file supplied as a first argument

exec < $archivoPalabras
let count=0

#Guardo las palabras de archivoa.txt en un array
while read word; do

    wordsA[$count]=$word
    ((count++))
done

#Inicializo count1 para iterar
#	noWords tiene la cantidad total de palabras en archivob.txt
let count1=0

noWords=$(cat $archivoBusco | wc -w)

#Itero para contar las coincidencias
#Tambien cuento las palabras que no estan en archivoA
for item in ${wordsA[*]}
do
	#grep -c $item ./archivob.txt cuenta las ocurrencias de $item
	ocurrencias[$count1]=$(grep -c $item ./archivob.txt)
	let noWords=noWords-ocurrencias[$count1]
	((count1++))	
done

# maxCant = cantidad palabras en archivoa.txt
maxCant=${#wordsA[*]}
i=0

#Imprimo el resultado final
while [ $i -ne $maxCant ]
do
	echo ${wordsA[$i]}':' ${ocurrencias[$i]}
	((i++))
done
echo 'No existen en A:' $noWords

# restore stdin from filedescriptor 10
# and close filedescriptor 10
exec 0<&10 10<&-