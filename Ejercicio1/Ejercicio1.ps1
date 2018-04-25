<#
    .SYNOPSIS
    Contador de archivos con numero en el path

    .NOTES
    Nombre del Script: Ejercicio2.ps1

    Trabajo Práctico Nro. 1 - Ejercicio 1

    Integrantes:
            Arana, Juan Pablo        33904497
            Gonzalez, Mauro Daniel   35368160
            Sapaya, Nicolás Martín   38319489

    Instancia de Entrega: Entrega
#>

Param($path)
$a = Get-ChildItem $path -File -Recurse
foreach ($item in $a) {
  if ($item.FullName -match ‘[0-9]’) {
    $contador++
} }
Write-Output $contador

<#

1.
El objetivo del script es contar todos archivos que contienen un número en el nombre completo, 
de todos directorios y subdirectorios del path pasado como parámetro


2.
Se prodría validar:
    - que se pase un parámetro
    - que el parámetro sea un string
    - se podría validar que el string tenga una longitud maxima o mínima



3. 
El operando "-match" sirve para búscar la ocurrencia de una expresión regular, en este caso particular se busca que haya un número.



4.
Se podría reemplazar el foreach de la siguiente manera

Param($path)
$contador = (Get-ChildItem $path -Recurse -File | Where-Object {$_.FullName -match '[0-9]'} | measure).Count
Write-Output $contador

#>
