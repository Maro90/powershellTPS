<#
.SYNOPSIS
Juego de Memoria con TimeOut.

.DESCRIPTION
Se muestra una palabra (3 segundos), si se escribe correctamente (en menos de 5 segundos), se muestra la primera y otra más, y así sucesivamente.
    
.PARAMETER pathPalabras
Path del archivo de palabras [OBLIGATORIO].
    
.PARAMETER pathPuntajes
Path del archivo de puntajes. [OPCIONAL].

.PARAMETER nombreJugador
Nombre del usuario que jugará [OBLIGATORIO].
    
.NOTES
Nombre del Script: Ejercicio6.ps1

Trabajo Práctico Nro. 1 - Ejercicio 6

Integrantes:
            Arana, Juan Pablo        33904497
            Gonzalez, Mauro Daniel   35368160
            Sapaya, Nicolás Martín   38319489

Instancia de Entrega: Primer Re Entrega
#>

Param([Parameter(Mandatory=$true)][string]$pathPalabras,
      [Parameter(Mandatory=$false)][string]$pathPuntajes = 'puntajes.csv',
      [Parameter(Mandatory=$true)][string]$nombreJugador
)

function Read-HostWithTimeout {
    <#
    .SYNOPSIS
    Text input with timeout.

    .DESCRIPTION
    Text input with timeout. If a key is not pressed before the configured timeout, input will end.
    
    .PARAMETER Prompt
    Prompt for input.  Will have a ': ' appended, but no linefeed.
    
    .PARAMETER Timeout
    Seconds to wait for input.
    
    .NOTES
    Based on https://blogs.msdn.microsoft.com/timid/2014/01/29/read-host-with-a-timeout-kind-of/
    #>

    Param (
        [Parameter(Mandatory = $false)]
        [string]$Prompt = '',
        [Parameter(Mandatory = $true)]
        [ValidateRange(1, 60)]
        [int]$Timeout
    );
    
    Write-Host "${Prompt}: " -NoNewline;
    $key = '';
    $first = $true;
    $end = (Get-Date).AddSeconds($Timeout)
    $fullString='';
    while ((Get-Date) -lt $end){
        if ($host.ui.RawUI.KeyAvailable){
            $key = $host.UI.RawUI.ReadKey("NoEcho, IncludeKeyUp").Character;
            if ($first -eq $false){
                Write-Host $key -NoNewline
                if ([System.Text.Encoding]::ASCII.GetBytes($key.ToString())[0] -eq 13 -and $first -eq $false){
                    Clear-Host
                    break;
                }
                if ([System.Text.Encoding]::ASCII.GetBytes($key.ToString())[0] -ne 8){
                    $fullString += $key;
                }else{
                   $fullString = $fullString.Remove(($fullString.Length)-1)
                   Clear-Host
                   Write-Host ": $fullString" -NoNewline
                }
                $end = (Get-Date).AddSeconds($Timeout)
            }else{
                $first = $false;                
            }
        }
        Start-Sleep -Milliseconds 50;        
    }
    return $fullString;
}


function saveTable{
    <#
    .SYNOPSIS
    Se encarga de la exportación de los puntajes.

    .DESCRIPTION
    Importa los puntajes, agrega el nuevo puntaje si ha de superar alguno de los otros, y lo exporta.
    
    .PARAMETER nombre
    Es el nombre del jugador.
    
    .PARAMETER point
    Es el puntaje actual del jugador.

    .PARAMETER pathPuntaje
    Es el path de donde se importarán los puntajes antiguos.
    #>

    Param (
        [Parameter(Mandatory = $true)]
        [string]$nombre,
        [Parameter(Mandatory = $true)]
        [int]$point,
        [Parameter(Mandatory = $true)]
        [string]$pathPuntaje
    )

    #Creo Arrays donde almaceno Nombres y Puntajes.
    $arrayNombre = @()
    $arrayPuntaje = @()

    if($notCreated -eq $false){
        #Importo los puntajes.
        $puntajes = Import-Csv -Path $pathPuntaje -Delimiter ";"
    }

    #Creo una tabla.
    $tableNew = New-Object system.Data.DataTable

    #Defino columnas.
    $colString = New-Object system.Data.DataColumn Nombre,([string])
    $colInt = New-Object system.Data.DataColumn Puntaje,([int])

    #Agrego columnas.
    $tableNew.columns.add($colString)
    $tableNew.columns.add($colInt)

    if($notCreated -eq $false){
        if($puntajes.Count -eq 3){
            #Uso los arrays para despues guardar todo en la tabla. (Si el puntaje nuevo supera alguno de los otros, lo inserto)
            forEach($item in $puntajes){
                if(([int]::Parse($item.Puntaje) -ge $point)){
                    $arrayNombre += $item.Nombre
                    $arrayPuntaje += ([int]::Parse($item.Puntaje))
                }else{
                    $arrayNombre += $nombre
                    $arrayPuntaje += $point
                    $nombre = $item.Nombre
                    $point = ([int]::Parse($item.Puntaje))
                }
            }
            for($i=0;$i -lt 3;$i++){
            $tableNew.Rows.Add($arrayNombre[$i],$arrayPuntaje[$i])
            }
        }else{
            #Si no hay 3 cargados, comparo y lo cargo directo
            $contador = 1
            forEach($item in $puntajes){
                if(([int]::Parse($item.Puntaje) -ge $point)){
                    $arrayNombre += $item.Nombre
                    $arrayPuntaje += ([int]::Parse($item.Puntaje))
                }else{
                    $arrayNombre += $nombre
                    $arrayPuntaje += $point
                    $nombre = $item.Nombre
                    $point = ([int]::Parse($item.Puntaje))
                }
                $contador++
            }
            $arrayNombre += $nombre
            $arrayPuntaje += $point
            for($i=0;$i -lt $contador;$i++){
                $tableNew.Rows.Add($arrayNombre[$i],$arrayPuntaje[$i])
            }
        }
    }else{
        $tableNew.Rows.Add($nombre,$point)
    }
    #Exporto el csv con los nuevos puntajes.

    $tableNew | Sort-Object -Property Puntaje -Descending | Export-Csv -Path $pathPuntaje -Delimiter ";"
}


function mostrarPalabra{
    <#
    .SYNOPSIS
    Se encarga de mostrar las palabras.

    .DESCRIPTION
    Se encarga de mostrar las palabras y ver si el usuario escribió correctamente la palabra/s mostrada/s.
    
    .PARAMETER allWords
    Son todas las palabras que contiene el archivo donde se encuentran las palabras.
    
    .PARAMETER arrayWords
    Son todas las palabras que se van seleccionando al azar.
    #>

    Param([Parameter(Mandatory=$true)][validateNotNullorEmpty()]$allWords,
          [Parameter(Mandatory=$true)][validateNotNullorEmpty()]$arrayWords
    )

    #Flag si escribio bien la palabra, elijo una palabra al azar de todas las que hay y la agrego al array, limpio la consola.
    $escribioBien = $true
    $arrayWords.Add((Get-Random $allWords))
    Clear-Host

    #Muestra la/s palabra/s (por un tiempo), a escribir, almacenadas en el array.
    Write-Host "Palabras a escribir:"
    while($escribioBien -eq $true){
        $escribioBien = $false
        forEach($item in $arrayWords){
            Write-Host "$item"  
        }
        $time = [System.Diagnostics.Stopwatch]::StartNew();
        while($time.Elapsed.TotalSeconds -lt $readTime){
        }
        $time.Stop();

        Clear-Host
        
        #Itero todas las palabras a escribir, si se equivoca, pierde. (se usa la funcion Read-HostWithTimeout para leer la palabra)
        while($escribioBien -eq $false){
            forEach($item in $arrayWords){
                if($item -eq ($input = Read-HostWithTimeout -Timeout $writeTime)){

                    $escribioBien = $true
                }else{

                    return $false
                }
            }
        }
        return $escribioBien
    }
}

function ConvertPath {
    <#
    .SYNOPSIS
    Cambia la última string del path.

    .DESCRIPTION
    Cambia el path de entrada, cambiando la última string por "puntajes.csv"
    
    .PARAMETER path
    Path de entrada.
    #>
    Param (
            [Parameter(Mandatory = $true)]
            [string]$path = ''
        );
    
    #Spliteo el path de palabras y creo un nuevo path que será el de puntajes.
    $splitPath = $path.Split('\') 
    $newPath = ''

    #Itero el path spliteado, pero cambio el archivo de palabras por el de puntajes y retorno el path de puntajes.
    forEach($item in $splitPath){
        if( $item -ne $splitPath.Get($splitPath.Count-1)){
            $newPath += $item + '\'
        }else{
            $newPath += 'puntajes.csv'
        }   
    }
    return $newPath
}

#Flag por si esta creado o no el archivo de puntajes para no hacer el import.
$notCreated = $false

#Guarda la direccion absoluta del documento
$palabrasFullPath = (Get-ChildItem $pathPalabras).fullName

#Testeo si el/los path/s ingresado/s es/son correcto/s.
if (-not (Test-Path $pathPalabras)){
    Write-Error 'El path de palabras es erroneo. [pathPuntajes], pathPalabras, nombreJugador'
    return;
}

if (-not (Test-Path $pathPuntajes)){
    if(($pathPuntajes -eq 'puntajes.csv') -or($pathPuntajes -eq (ConvertPath -path $palabrasFullPath))){
        $notCreated = $true
    }else{
        Write-Error 'El path de puntajes es erroneo. [pathPuntajes], pathPalabras, nombreJugador'
        return;
    }
}

#Establezco los tiempos en que muestra las palabras, y el time out para escribir.
$readTime = 3
$writeTime = 5

#Creo la variable donde almacenará los puntos, obtengo todas las palabras del archivo.
$points = -1
$allWords = Get-Content $pathPalabras

#Verifica si el archivo de palabras estaba vacio.
if($allWords -eq $null){
    Write-Error 'El archivo de palabras esta vacio.'
    return;
}

#Creo un array donde estarán todas las palabras elegidas al azar.
$arrayWords = New-Object System.Collections.ArrayList

#Mientras que escriba bien todas las palabras, continuara sumando puntos.
do{
    $result = mostrarPalabra -allWords $allWords -arrayWords $arrayWords; 
    $points++;
}while($result[1] -eq $true)

#Ha perdido muestro el puntaje del momento.
#Clear-Host
Write-Output "Juego Terminado, Score: $points"

#Dependiendo si se ingreso path de puntaje o si es path relativo, llama o no a convertir el path.
if((($pathPalabras -eq 'palabras.txt') -or ($pathPalabras -eq '.\palabras.txt')) -or ($pathPuntajes -ne 'puntajes.csv')){
    saveTable -nombre $nombreJugador -point $points -pathPuntaje $pathPuntajes
}else{
    $pathPuntajes = ConvertPath -path $palabrasFullPath
    saveTable -nombre $nombreJugador -point $points -pathPuntaje $pathPuntajes
}