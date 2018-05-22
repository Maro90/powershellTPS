signal_SIGUSR1(){    
    directory=`echo $PATH_ENTRADA | grep -o '[^/]*$'`

    DIA=`date +"%d%m%Y"`
    HORA=`date +"%H%M"`
    DATE=`date +"%d%m%Y %H%M"`

    fileName="$directory$DIA$HORA"
    zipfile="${PATH_SALIDA}$fileName.zip"

    count=`zip $zipfile ${PATH_ENTRADA}/* | wc -l`
    echo "Se comprimieron $count archivos el $DATE" >> logFile
    
}

signal_SIGUSR2(){
    echo ${PATH_SALIDA} > ./midoc.txt        #Se debe agregar al ~/.bash_profile 
}

signal_SIGTERM(){
    echo "Señal SIGTERM"
    exit
}

#Capturo las señales SIGUSR1 y SIGUSR2
#   
#   Para enviarselas al proceso   "kill -s SIGUSR1 [PID]  -> se busca con  ps "
#
trap signal_SIGUSR1 SIGUSR1
trap signal_SIGUSR2 SIGUSR2

#Señal para terminar
trap signal_SIGTERM SIGTERM

#ignorando las siguientes señales
#trap "" SIGHUP
#trap "" SIGINT
#trap "" SIGQUIT
#trap "" SIGABRT
#trap "" SIGALRM

logFile=$1

while true
do
    echo "Soy un bucle" >>  /dev/null
done
