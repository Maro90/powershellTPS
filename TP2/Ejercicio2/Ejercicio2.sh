#!/bin/bash

if [ $# != 2 ]; then
    echo "Debe pasar dos archivos por parametro"
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
cp $1 $1.bak

ARCH1=/tmp/`basename $1`.$$

cp $1 $ARCH1

PMAYUS=$(sed -r 's/(.*)/s\/\1\/\1\/I;/' $2)
REPLACE="
s/[A-Z]*/\L&/g
s/^./\u&/
s/([\.;,])\s*([a-z])/\1 \2/g
s/(\s+)/ /g
s/\.\s+$/./
$PMAYUS
"

sed -i -r "$REPLACE" $ARCH1

mv $ARCH1 $1