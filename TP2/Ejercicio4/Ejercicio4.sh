#!/bin/bash

#################################################
#             Sistemas Operativos               #
#       Trabajo Práctico 2 - Ejericio 4         #
#       Nombre del Script: Ejercicio4.sh        #
#                                               #
#               Integrantes:                    #
#       Arana, Juan Pablo       33904497        #
#       Gonzalez, Mauro Daniel  35368160        #
#       Sapaya, Nicolás Martín  38319489        #
#                                               #
#       Instancia de Entrega: Entrega           #
#                                               #
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
	
    echo "$0 [Archivo] -r"
	echo "$0 [Archivo] -r -p"
	echo "$0 [Archivo] -l [Legajo]"

	exit	
}

#***********************************************************************************************

#Funcion si el archivo de registros no existe, no tiene permisos de lectura o esta vacío.

ErrorVacioInex() { 
	clear
	if [ ! -e "$1" ]; then
        echo "'$1' no existe."
	elif [ ! -r "$1" ]; then
		echo "No tiene permisos de lectura para '$1'."
	elif [ ! -s "$1" ]; then
		echo "'$1' es vacio."
	fi
	exit	
}

#***********************************************************************************************

#Obtener mes y año a partir del path del archivo que se utilizarán de manera global

SetDate(){
    filename=$(basename -- "$1")
    extension="${filename##*.}"
    filename="${filename%.*}"
    filenameParts=(${filename//_/ })
    date=${filenameParts[1]}
    ano=`echo $date | cut -c1-4`
    mes=`echo $date | cut -c5-6`
}

#***********************************************************************************************

#Funcion para obtener la cantidad de segundos hasta la hora pasada por parametros
#La hora es con el formato HH:MM:SS

CalcularTimestamp(){
    clockParts=(${1//:/ })
    horas=${clockParts[0]}
    minutos=${clockParts[1]}
    segundos=${clockParts[2]}
    horasASegundos=`expr $horas \* 60 \* 60`
    minutosASegundos=`expr $minutos \* 60`
    let "segundosTotales=$horasASegundos + $minutosASegundos + $segundos"
    echo "$segundosTotales"
}

#***********************************************************************************************

#Funcion para obtener la hora en formato HH:MM a partir de la cantidad de segundos pasadas por parametro

ObtenerHorasDesdeTimeStamp(){
    let "horas= $1 / 60 / 60"
    let "minutosRestantes= $1 / 60"
    minutos=`expr $minutosRestantes - $horas \* 60`
    echo "$(printf %02d $horas):$(printf %02d $minutos)"
}

#***********************************************************************************************

#Funcion imprimir los calculos totales de los registros

ImprimirHorasTotales(){
    echo -e "Total de horas teóricas: $horasTeoricas:00"     #La cantidad de horas al mes se calcula siempre rendonda, por lo cual damos por hecho que los minutos son 00
    echo -e "Total de horas trabajadas: $horasTrabajadasReales"
    echo -e "Horas Extra: $horasExtra"

}

#***********************************************************************************************

#Funcion para realizar el calculo de horas trabajadas del legajo pasado por parámetro.

EvaluarRegistrosDelLegajo(){
    legajo=$1
    #genero archivo para el reporte
    regisFile=$(dirname -- "$archivoDeRegistros")"/$legajo""_$ano$mes.reg"
    shift

    tiempoTotalTrabajado=0
    #cargo archivo con la cabecera
    if $generarArchivos;then          
        echo -e "Legajo \t\tFecha \t\t\tIngreso \tEgreso \t\tHoras Trabajadas" > "$regisFile"
        echo "" >> "$regisFile"
    fi

    for info in $@;
    do
        registro=(${info//;/ })
        if test $legajo == ${registro[0]}; then

            #calculos de los reportes

            dia=$(printf %02d ${registro[1]})
            timeEntrada=`CalcularTimestamp ${registro[2]}`
            timeSalida=`CalcularTimestamp ${registro[3]}`

            let "segundosTrabajados = $timeSalida - $timeEntrada"
            tiempoTrabajado=`ObtenerHorasDesdeTimeStamp $segundosTrabajados`

            let "tiempoTotalTrabajado=$tiempoTotalTrabajado + $segundosTrabajados"
            if $generarArchivos;then         
                #cargo archivo con los datos de los registros 
                echo -e "${registro[0]} \t\t$dia/$mes/$ano  \t${registro[2]}  \t${registro[3]} \t\t$tiempoTrabajado" >> "$regisFile"
            fi
        fi
    done
                
    if $generarArchivos;then          
        echo "---------------------------------------------------------------------------------------------" >> "$regisFile" 
    fi
    
    horasTeoricas=`expr 22 \* 8`
    horasTrabajadasReales=`ObtenerHorasDesdeTimeStamp $tiempoTotalTrabajado`
    tiempstampHorasTeoricas=`expr $horasTeoricas \* 60 \* 60`
    let "tiempoExtra = $tiempoTotalTrabajado - $tiempstampHorasTeoricas"

    if test $tiempoExtra -le 0; then
        tiempoExtra=0
    fi
    horasExtra=`ObtenerHorasDesdeTimeStamp $tiempoExtra`
    
    if $generarArchivos; then
        ImprimirHorasTotales >>"$regisFile"
    fi
    if $mostrarLegajosProcesados; then
        echo "Legajo: $legajo"
        echo
        ImprimirHorasTotales
    fi

    return  
}
#***********************************************************************************************

#Funcion para validar la existencia de un legajo

ValidarLegajo(){
    legajo=$1
    shift

    for info in $@;
    do
        registro=(${info//;/ })
        if test $legajo == ${registro[0]}; then
            return
        fi
    done
    #Legajo Inexistente
	echo "El legajo solicitado no existe en los registros: $legajo"
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
if (test $# -lt 2 || test $# -gt 3); then 	
	ErrorSintaxOHelp 1
fi
    
#Siempre debe ser -r o -l los parametros
if test $2 != "-r" && test $2 != "-l"; then
    ErrorSintaxOHelp 1
fi

#Si pasa -l debe ingresar un legajo.
if  test $2 == "-l" && test $# -lt 3;then
    ErrorSintaxOHelp 1
fi

if (test -r "$1" && test -s "$1"); then

    archivoDeRegistros="$1"
    generarArchivos=true
    mostrarLegajosProcesados=false

    #Se carga la fecha
    SetDate "$1"

    #Se analiza el archivo y se extraen los datos
    linea=0
    countLegajos=0
    for line in $(cat "$1"); 
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

    if test $2 == "-l";then
        ValidarLegajo $3 ${datos[*]}
        generarArchivos=false
        mostrarLegajosProcesados=true
        EvaluarRegistrosDelLegajo $3 ${datos[*]}
    else
        #El segundo parametro era '-l' o '-r', previamente verificado.
        #Siendo el segundo '-r', y teniendo 3 parámetros debe ser '-p'.
        if test $# == 3;then
            if test $3 == "-p";then
                mostrarLegajosProcesados=true
            else
                ErrorSintaxOHelp 1
            fi
        fi

        for legajoId in ${legajos[*]};
        do
            EvaluarRegistrosDelLegajo $legajoId ${datos[*]}
            if $mostrarLegajosProcesados; then
                echo "---------------------------------------------------------------------------------------------" 
            fi
        done
    fi
else
    #Archivo Inexistente o No se puede leer
	ErrorVacioInex "$1"
fi