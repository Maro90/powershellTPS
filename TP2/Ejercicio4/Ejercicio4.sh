#!/bin/bash

#Funcion si se ingresaro parametros incorrectos.
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

ErrorVacioInex() { 
	#clear
	if [ ! -e $1 ]; then
        echo "'$1' no existe."
	elif [ ! -r $1 ]; then
		echo "No tiene permisos de lectura para '$1'."
	elif [ ! -s $1 ]; then
		echo "'$1' es vacio."
	fi
	exit	
}

#Se comprueba que la cantidad de parametros este entre 1 y 3
if (test $# -lt 2 || test $# -gt 3); then 	
	ErrorSintaxOHelp 1
fi

if test $1 == "-h" || test $1 == "--help" || test $1 == "-?"; then 
	#Es -h -help o -?
	ErrorSintaxOHelp 0
fi


ImprimirLegajo(){
    echo -e "params $@ \n"

    legajo=$1
    shift

    echo -e "Legajo \tFecha \t\tIngreso \t\tEgreso \t\tHoras Trabajadas"
    for datos in $@;
    do
        registro=(${datos//;/ })
        if test $legajo == ${registro[0]}; then
            echo -e "${registro[0]} \t${registro[1]}  \t\t${registro[2]}  \t\t${registro[3]}"
        fi
    done    
}









if (test -r $1 && test -s $1); then
    #Siempre debe ser -r o -l los parametros
    if test $2 != "-r" && test $2 != "-l"; then
        ErrorSintaxOHelp 1
    fi

    
    linea=0
    countLegajos=0
    for line in $(cat $1); 
    do
        if test $line != "Legajo;dia;ingreso;egreso";then
            #No consideramos esta linea en caso de estar

            registro=(${line//;/ })
            datos[linea]=$line
            newOne=true

            #Se arma listado de legajos
            for ((i=0;i<countLegajos;i++))
            do
                #si ya existe el legajo no lo agrega
                if (test ${legajos[$i]} == ${registro[0]});then
                    newOne=false
                fi
            done
            if $newOne; then
                legajos[$countLegajos]=${registro[0]}
                countLegajos=$((countLegajos+1))
            fi

            linea=$((linea+1))
        fi
    done


    for legajo in ${legajos[*]};
    do
        echo -e "params ${datos[*]} \n"

        #TODO:  revisar que la segunda vez llega mal

        ImprimirLegajo $legajo ${datos[*]}
        echo -e "\n"

    done


else
    #Archivo Inexistente o No se puede leer
	ErrorVacioInex $1
fi