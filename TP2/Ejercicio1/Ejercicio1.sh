#!/bin/bash

if [ $# != 1 ]; then
    echo "Debe pasar un archivo como parametro"
    exit
fi
if ! [ -f "$1" ]; then
    echo "El archivo no es válido "
    exit 
fi
file -i "$1" | grep text/plain >> /dev/null
if [ $? != 0 ]
then
    echo "Formato de archivo es invalido"
    exit 
fi
cp $1 $1.bak
ARCH1=/tmp/`basename $1`.$$
sed 's/[A-Z]*/\L&/g' $1 > $ARCH1
sed -i 's/^./\u&/' $ARCH1
mv $ARCH1 $1


# 1 - Esta linea le indica al shell que interprete debe usarse, el interprete en este caso es Bash (podria ser sh o ksh por ejemplo)

# 2 - Le otorgamos permisos al script con el comando "chmod", utilizando el  chmod 777 Ejercicio1.sh 
# con lo cual se setea permisos de lectura, escritura y ejecucion para todos los grupos de usuarios

# 3 - $1 contiene el primer parametro pasado al script, los siguientes (separados por espacios) serán enumerado $2 $3, 
# así como el nombre del script es $0.
# $? devuelve la salida del pipe más reciente. Y $# devuelve el numero total de parametros pasados al script.
# También hay otros similares como por ejemplo:
# $* que devuelve todos los parametros pasados al script como string
# $_ devuelve el ultimo parametro pasado al ultimo comando ejecutado
# $@ que devuelve todos los parametros pasados al script como lista
# $$ que devuelve el pid de la shell actual
# $! que devuelve el pid del ultimo comando ejecutado en segundo plano
# $- que devuelve la lista de opciones en la shell actual

#4 - El objetivo del script es dejar la primer letra de cada linea en mayuscula y el resto en minuscula

#5 - Sed es un editor de texto que reemplaza segun la expresion en sus parametros.
#    - i edita el archivo sin imprimirlo en la salida estandar

#6 - El script no funciona si el archivo tiene espacios ya que seria tomado como varios parametros.
# para solucionarlo podrian agregarse comillas 
