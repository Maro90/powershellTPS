<#
    .SYNOPSIS
    Nombre del Script: Ejercicio3.ps1

    Trabajo Práctico Nro. 1 - Ejercicio 3

    Integrantes:
                Arana, Juan Pablo        33904497
                Gonzalez, Mauro Daniel   35368160
                Sapaya, Nicolás Martín   38319489

    Instancia de Entrega: Entrega


    .DESCRIPTION   
#>
    
Param(
    [parameter(Mandatory=$true,Position=1)]
    [string]$filePath,
    [parameter(Mandatory=$true,ParameterSetName="setU")]
    [switch]$U,
    [parameter(Mandatory=$true,ParameterSetName="setC")]
    [switch]$C,
    [parameter(Mandatory=$true,ParameterSetName="setK")]
    [switch]$K
)

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
        $procesos+=$process
    }else {
        #en caso de que no se haya podido encontrar el proceso se loguea el evento
        Add-Content -path $logFilePath "No se pudo obtener informacion del proceso '$item'."
        Add-Content -path $logFilePath ""
    }
}

#se evalua la interaccion a lleva a cabo teniendo en cuenta el segundo parametro de tipo switch asociado a alguno de los grupos
switch ($PSCmdlet.ParameterSetName) {
    'setU' {

        foreach ($item in $procesos) {
            #por cada proceso encontrado se loguea su nombre y ID
            $item | Format-List -Property Name,id
        }
    }
    'setC' {
        Write-Output 'C'
    }
    'setK'{
        Write-Output 'K'
    }
}

