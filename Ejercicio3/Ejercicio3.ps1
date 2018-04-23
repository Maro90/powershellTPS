<#
    .SYNOPSIS
    C.

    .DESCRIPTION
    Crea un Job que cada vez que se cierra el programa editor de texto pasado, evalua el documento y crea una nueva version cambiando las letras a mayusculas en todo inicio de oracion que corresponda, y actualiza un log con los numeros de caracteres cambiados y la fecha.
    
    .PARAMETER documentPath
    Path al documento que debe ser evaluado cuando se cierra el programa editor.
    
    .PARAMETER logPath
    Path al documento de log que debe ser actualizado o creado si no existe.
    
    .PARAMETER programName
    Nombre del programa que se va a utilizar para editar el archivo.
#>

<#
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

function checkDocument () {
    $texto = (Get-Content $documentPath)
    $shouldChange = $true
    $lineNumber = 0
    $charactersChanged = 0
    
    foreach ($item in $texto) {    
        $count = $item.length
    
        if($item -notmatch '.'){
            #cases with only a break line
            $newTexto += "`n"
        } else{
            if($lineNumber -ne 0){
                #adding break line after each line, except by the last one
                $newTexto += "`n"
            }
            for($index = 0; $index -lt $count; $index++){
                $letter = $item[$index]
                
                switch -regex ($letter) {
                    #Checking for alphabetic char
                    '[a-zA-Z]' {
                        if($shouldChange -eq $true){
    
                            if($letter -cmatch '[a-z]'){
                                #Here I must make upper case the char, because it is lowercase
                                $letter = ($letter).ToString().ToUpper()
                                $charactersChanged++
                            } 
                            $shouldChange = $false    
                        }
                        break
                    }
                    #Checking for "."
                    '\.'{
                        $shouldChange = $true
                        break
                    }
                    #Checking for any character except newline
                    '.'{
                        break
                    }
                    default{
                        #unknowned "break line"
                        Write-Output 'unknowned'
                    }
                }
                $newTexto += $letter
            }
        }
        $lineNumber++
    }
    
    #Creating updated file
    $directory = Split-Path -Parent $documentPath

#    $fileName = Split-Path -LeafBase $documentPath
#    $fileExtension = Split-Path -Extension $documentPath
    $fileExtension = (dir $documentPath).Extension 
    $fileName = (dir $documentPath).BaseName

    $newFile = "$fileName"+"_modif$fileExtension"
    $newFilePath = "$directory/$newFile"
    Set-Content -path $newFilePath $newTexto
    
    #Generating Log
    $date = Get-Date -UFormat "%A, %d/%m/%Y %H:%M:%S"
    
    $log=""
    $log+="----------------------------------------------------------`n"
    $log+="FileName:`t`t`t`t$fileName$fileExtension`n"
    $log+="Updated FileName:`t`t$newFile`n"
    $log+="Characters Changed:`t`t$charactersChanged`n"
    $log+="Date:`t`t`t`t`t$date`n"
    
    if (Test-Path $logPath){ #Update log
        Add-Content -path $logPath $log  
    }else{  #Create Log
        Set-Content -path $logPath $log
    }    
}

$process = Start-Process $program -FilePath $documentPath -PassThru

$job = Register-ObjectEvent -InputObject $process -EventName exited -Action {
    Get-EventSubscriber | Unregister-Event
    checkDocument
}

Start-Job -InputObject $job -ScriptBlock{
    checkDocument
}