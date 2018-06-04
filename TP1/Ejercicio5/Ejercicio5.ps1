<#
    .SYNOPSIS
    Analizador de procesos

    .DESCRIPTION
    Se encarga de analizar los procesos pasado e interactuar de a cuerdo a los parametros pasados.
    Soporta tres parametros exclusivos para interactuar
    
    .PARAMETER filePath
    Path al documento que contiene los procesos que se van a evaluar.
    
    .PARAMETER U
    Switch exclusivo, si se utiliza se mostrara el PID y los porcentajes de uso de CPU y de la memoria.
    
    .PARAMETER C
    Switch exclusivo, si se utiliza se mostrara el PID y el porcentaje de uso de CPU.

    .PARAMETER C
    Switch exclusivo, si se utiliza matara los procesos pasados.


    .Example
    .\Ejercicio5.ps1 -filePath .\data\procesos.txt -U

    .Example
    .\Ejercicio5.ps1 C:\data\procesos.txt -K

    .Notes
    Nombre del Script: Ejercicio5.ps1

    Trabajo Prctico Nro. 1 - Ejercicio 5

    Integrantes:
                Arana, Juan Pablo        33904497
                Gonzalez, Mauro Daniel   35368160
                Sapaya, Nicols Martn   38319489

    Instancia de Entrega: 1er Re Entrega

#>
    
Param(
    [parameter(Mandatory=$true,Position=1)]
    [validateNotNullorEmpty()]
    [string]$filePath,
    [parameter(Mandatory=$true,ParameterSetName="setU")]
    [switch]$U,
    [parameter(Mandatory=$true,ParameterSetName="setC")]
    [switch]$C,
    [parameter(Mandatory=$true,ParameterSetName="setK")]
    [switch]$K
)

if ((Test-Path $filePath) -eq $false){
    Write-Output "Su archivo no es existe."
    return
}


#Se obtienen los procesos con los cuales se va a interactuar
$texto = (Get-Content $filePath)

#Se crea e inicializa el archivo de log
$logFilePath = '.\log.txt'
Set-Content -path $logFilePath ''

#Se crea funcion para obtener el proceso
function getproceso{

    #Se vincula el script para poder capturar los errores
    [CmdletBinding()]
    
    Param(
        [parameter(ValueFromPipeline=$true)]
        [string]$name
    )

    #Se obtiene el nombre del proceso sin su extension
    $pname = [io.path]::GetFileNameWithoutExtension($name)
    
    try {
        #Se busca el proceso asociado al nombre dado
        $process = Get-Process $pname -EV errorGet -EA stop
    }
    catch {

        #Se capturan los errores en caso de suceder, se informa de ello en el archivo de logs
        Add-Content -path $logFilePath "El proceso de '$name' no se puede encontrar, es posible que no este corriendo."
        #Se descarta el error capturado y debidamente informado en el archivo de log
        Write-Output $errorGet | Out-Null

        return $false
    }

    #se devuelve el proceso encontrado
    return $process
}

#se genera array para trabajar con los procesos encontrados
$procesos = @()

foreach ($item in $texto) {
    #se buscan los procesos del documento
    $process = getproceso $item

    #se agregan los procesos encontrados al array
    if($process){
        $procesos+=([PSCustomObject]@{ name = $item; proceso = $process})
    }else {
        #en caso de que no se haya podido encontrar el proceso se loguea el evento
        Add-Content -path $logFilePath "No se pudo obtener informacion del proceso '$item'."
        Add-Content -path $logFilePath ""
    }
}

#se obtine el numero de procesadores de la computadora
$numberOfProcessors = (Get-WmiObject -Class Win32_Processor).NumberOfLogicalProcessors

#se evalua la interaccion a lleva a cabo teniendo en cuenta el segundo parametro de tipo switch asociado a alguno de los grupos
switch ($PSCmdlet.ParameterSetName) {
    'setU' {

        foreach ($item in $procesos) {
            #por cada proceso encontrado se loguea su nombre y ID

            $name = $item.name
            $processId = $item.proceso.ID
            
            #Se valida los casos donde hay mas de una instancia del proceso.
            if($item.proceso.Count -ne 1){
                $process = $item.proceso[0].name
                $memoryUse = 0 

                foreach($p in $item.proceso){
                    $memoryUse += $p.WorkingSet 
                }

            }else{
                $process = $item.proceso.name
                #Se obtiene la cantidad de memoria utizada por el proceso
                $memoryUse = $item.proceso.WorkingSet 

            }

            try {
                #Se obtiene el porcentaje de tiempo de uso del procesador
                $processorUse = Get-Counter -Counter "\proceso($process)\% de tiempo de procesador" -EV errorGet -EA stop | % { $_.countersamples.cookedvalue }
            }
            catch [System.Exception] {
                #Se capturan los errores en caso de suceder, se informa de ello en el archivo de logs
                Add-Content -path $logFilePath "No se puede acceder al uso del procesador del proceso de '$name'. Debe ser un problema del idioma de tu maquina"
                #Se descarta el error capturado y debidamente informado en el archivo de log
                Write-Output $errorGet | Out-Null
            }            
            

            #Se calcula el uso del procesador en base a la cantidad de procesadores
            [decimal]$realUseCPU = $processorUse / $numberOfProcessors
            $realUseCPU = [math]::Round($realUseCPU,3)

            [decimal]$memoryUse = $memoryUse / 1024

            Write-Output "------------------------------------------"
            Write-Output "Proceso: $name"
            Write-Output "Pid: $processId"
            Write-Output "Uso de CPU: $realUseCPU%"
            Write-Output "Uso de Memoria: $realUseMemory KB"
            
            Write-Output "------------------------------------------"

        }
    }
    'setC' {
        foreach ($item in $procesos) {
            #por cada proceso encontrado se loguea su nombre y ID

            $name = $item.name
            $processId = $item.proceso.ID
            
            #Se valida los casos donde hay mas de una instancia del proceso.
            if($item.process.Count -ne 1){
                $process = $item.proceso[0].name
            }else{
                $process = $item.proceso.name
            }            

            try {
                #Se obtiene el porcentaje de tiempo de uso del procesador
                $processorUse = Get-Counter "\proceso($process)\% de tiempo de procesador" -EV errorGet -EA stop | % { $_.countersamples.cookedvalue }
            }
            catch [System.Exception] {
                #Se capturan los errores en caso de suceder, se informa de ello en el archivo de logs
                Add-Content -path $logFilePath "No se puede acceder al uso del procesador del proceso de '$name'. Debe ser un problema del idioma de tu maquina"
                #Se descarta el error capturado y debidamente informado en el archivo de log
                Write-Output $errorGet | Out-Null
            }

            #Se calcula el uso del procesador en base a la cantidad de procesadores
            [decimal]$realUse = $processorUse / $numberOfProcessors
            $realUse = [math]::Round($realUse,3)

            Write-Output "------------------------------------------"
            Write-Output "Proceso: $name"
            Write-Output "Pid: $processId"
            Write-Output "Uso de CPU: $realUse%"
            
            Write-Output "------------------------------------------"

        }
    }
    'setK'{
        foreach ($item in $procesos) {
            #Se mata los procesos que realmente estan corriendo
            try {
                Stop-Process $item.proceso -EV errorGet -EA stop
            }
            catch {
                $processName = $item.name
                #Se capturan los errores en caso de suceder, se informa de ello en el archivo de logs
                Add-Content -path $logFilePath "El proceso de '$processName' no se puede detener, es posible que le pertenezca a otro usuario. Y tengas acceso denegado"
                #Se descarta el error capturado y debidamente informado en el archivo de log
                Write-Output $errorGet | Out-Null
            }        
        }
    }
}

