signal_SIGUSR1(){    
    echo "---------------------------------------------------------------------" >> $logFile
    echo "" >> $logFile

    directory=`echo $PATH_ENTRADA | grep -o '[^/]*$'`

    DIA=`date +"%d%m%Y"`
    HORA=`date +"%H%M"`
    DATE=`date +"%d/%m/%Y %H:%M"`

    fileName="$directory$DIA$HORA"
    zipfile="${PATH_SALIDA}/$fileName.zip"

    count=`zip $zipfile ${PATH_ENTRADA}/* | wc -l`
    echo "Se comprimieron $count archivos el $DATE" >> $logFile

    du -k $zipfile | awk '
    BEGIN {
        suma=0
    }
    {
        #print;# imprimir línea, tal y como lo hace du
        suma += $1; # calculamos la suma parcial
    }
    END {
        print "“El tamaño del archivo comprimido es "suma "kb";
    }' >> $logFile
    
}

signal_SIGUSR2(){
    echo "---------------------------------------------------------------------" >> $logFile
    echo "" >> $logFile

    outputPath="${PATH_SALIDA}/*"

    DATE=`date +"%H:%M %d/%m/%Y"`
    du -k $outputPath | awk '
        BEGIN {
            suma=0
            cant=0
        }
        {
            suma += $1; # calculamos la suma parcial
            cant=cant+1
        }
        END {
            print "Se eliminaron "cant " archivos y se liberaron "suma " kb";
        }' >> $logFile
    echo "A las $DATE" >> $logFile
    rm -fR $outputPath
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
    sleep 15s # Waits 15 seconds
done
