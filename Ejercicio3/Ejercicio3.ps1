<#
    .SYNOPSIS
    Actualiza las primeras letras de cada oracion a mayusculas

    .DESCRIPTION
    Abre el documento pasado por parametro con el editor pasado por parametro, en caso de no existir el documento lo crea.
    Cuando se cierra el editor revisa el documento actualiza las letras que hayan quedado en minusculas despues de un punto y crea un nuevo archivo,
    en el mismo directorio del documento pasado con el subfijo _modif
    Y crea un archivo de con el nombre y path pasados por parametros para loguear los cambios realizados.
    En caso de existir este archivo, solo agrega los nuevos cambios.
    
    .PARAMETER documentPath
    Path al documento que debe ser evaluado cuando se cierra el programa editor. Se crea si no existe.
    
    .PARAMETER logPath
    Path al documento de log que debe ser actualizado o creado si no existe.
    
    .PARAMETER program
    Nombre del programa que se va a utilizar para editar el archivo.

    .Example
    .\Ejercicio3.ps1 -documentPath .\data\midocumento.txt -logPath .\data\log.txt -programName notepad.exe

    .Example
    .\Ejercicio3.ps1 -documentPath C:\miPrograma\data\cv.txt -logPath C:\miPrograma\data\log.txt -programName notepad.exe
   
    .Notes
    Nombre del Script: Ejercicio3.ps1

    Trabajo Práctico Nro. 1 - Ejercicio 3

    Integrantes:
                Arana, Juan Pablo        33904497
                Gonzalez, Mauro Daniel   35368160
                Sapaya, Nicolás Martín   38319489

    Instancia de Entrega: Entrega


    #>

Param(
    [parameter(Mandatory=$true)]
    [validateNotNullorEmpty()]
    [string]$documentPath,
    [parameter(Mandatory=$true)]
    [validateNotNullorEmpty()]
    [string]$logPath,
    [parameter(Mandatory=$true)]
    [validateNotNullorEmpty()]
    [string]$program
)

$func ={
    function checkDocument{
        param(
            [parameter(Mandatory=$true,ValueFromPipeline=$true)]
            [string]$documentPath,
            [parameter(Mandatory=$true,ValueFromPipeline=$true)]
            [string]$logPath
        )
        
        $texto = (Get-Content $documentPath)
        $shouldChange = $true
        $lineNumber = 0
        $charactersChanged = 0
        
        foreach ($item in $texto) {    
            $count = $item.length
        
            if($item -notmatch '.'){
                #Casos con linea que son solo salto de linea
                $newTexto += "`n"
            } else{
                if($lineNumber -ne 0){
                    #Agrega un salto de linea despues de cada linea, excepto en la primera
                    $newTexto += "`n"
                }
                for($index = 0; $index -lt $count; $index++){
                    $letter = $item[$index]
                    
                    switch -regex ($letter) {
                        #Espera por letras
                        '[a-zA-Z]' {
                            if($shouldChange -eq $true){
        
                                if($letter -cmatch '[a-z]'){
                                    #Aqui debe cambiar la minuscula por mayuscula
                                    $letter = ($letter).ToString().ToUpper()
                                    $charactersChanged++
                                } 
                                $shouldChange = $false    
                            }
                            break
                        }
                        #Espera por el caracter '.'
                        '\.'{
                            $shouldChange = $true
                            break
                        }
                        #Espera por cualquier caracter salvo, el salto de linea
                        '.'{
                            break
                        }
                    }
                    $newTexto += $letter
                }
            }
            $lineNumber++
        }
        #Armado de path para el archivo modficado
        #Creando archivo con los cambios para el documento
        $directory = Split-Path -Parent $documentPath
    
        $fileExtension = (dir $documentPath).Extension 
        $fileName = (dir $documentPath).BaseName
    
        $newFile = "$fileName"+"_modif$fileExtension"
        $newFilePath = "$directory/$newFile"
        Set-Content -path $newFilePath $newTexto
        
        #Generando Log
        $date = Get-Date -UFormat "%A, %d/%m/%Y %H:%M:%S"
        
        $log=""
        $log+="----------------------------------------------------------`n"
        $log+="FileName:`t`t`t`t$fileName$fileExtension`n"
        $log+="Updated FileName:`t`t$newFile`n"
        $log+="Characters Changed:`t`t$charactersChanged`n"
        $log+="Date:`t`t`t`t`t$date`n"
        
        #Actualizando archivo de log
        Add-Content -path $logPath $log
    }
}
#Revisa que exista el documento y el archivo de log
$logExists = Test-Path $logPath
$documentExist = Test-Path $documentPath

#Si el documento o el archivo de log no existieran, los crea
if($logExists -ne $true){
    New-Item -path $logPath -ItemType 'file'
}
if($documentExist -ne $true){
    New-Item -path $documentPath -ItemType 'file'
}

#Guarda la direccion absoluta del documento y del archivo de log
$logFullpath = (Get-ChildItem $logPath).fullName
$documentFullpath = (Get-ChildItem $documentPath).fullName

#Crea el EventMessageObject para enviarselo al proceso que escucha el evento del programa
$eventData = [PSCustomObject]@{
    document = $documentFullpath;
    log = $logFullpath;
    func=$func
}

#Lanza el proceso del programa solicitado apuntando al documento pedido
$process = Start-Process $program -FilePath $documentPath -PassThru

#Se Registra al evento de cerrado del programa solicitado, y pasa por parametro los documentos que va a requerir
$job = Register-ObjectEvent -InputObject $process -EventName Exited -MessageData $eventData -Action {
    
    #Se desuscribe del evento del proceso
    Get-EventSubscriber | Unregister-Event;

    #Crea un job para revisar el documento,  
    Start-Job -InputObject $job -InitializationScript $event.MessageData.func -ScriptBlock { 

        #Ejecuta la funcion para revisar el documento pasandole el archivo a revisar y el archivo de logs
        checkDocument -documentPath $args[0] -logPath $args[1]
    
    #espera a que finalice y para recibir la informacion del job
    } -ArgumentList $event.MessageData.document,$event.MessageData.log | Wait-Job | Receive-Job
}