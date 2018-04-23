<#
    .SYNOPSIS
    C.

    .DESCRIPTION
    Create a Job which check the document, creates a new version with uppercase in every new sentence,
    and update a log each time the program is closed
    
    .PARAMETER documentPath
    Path to the document which will be updated after close the editor program.
    
    .PARAMETER logPath
    Path where will be the information about the number of characters changed and the date.
    
    .PARAMETER programName
    Name of the program which will be used to edit the file.
#>


Param(
    [parameter(Mandatory=$true)]
    [validateNotNullorEmpty()]
    [string]$documentPath,
    [parameter(Mandatory=$true)]
    [validateNotNullorEmpty()]
    [string]$logPath
)

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
$fileName = Split-Path -LeafBase $documentPath
$fileExtension = Split-Path -Extension $documentPath
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





<#
$job = Start-Job -ScriptBlock ({

})
#>