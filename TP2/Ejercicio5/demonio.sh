signal_SIGUSR1(){
    echo "Señal SIGUSR1"
    
    directorioAComprimir=${PATH_ENTRADA}
    filenameParts=(${directorioAComprimir//_/ })

    zip ${PATH_SALIDA}/ ${PATH_ENTRADA}/*
}

signal_SIGUSR2(){
    echo "Señal SIGUSR2"
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

while true
do
    echo "Soy un bucle" >>  /dev/null
done
