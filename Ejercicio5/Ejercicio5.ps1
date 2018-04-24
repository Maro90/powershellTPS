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
    [parameter(Mandatory=$true,ParameterSetName="SET1")]
    [switch]$U,
    [parameter(Mandatory=$true,ParameterSetName="SET2")]
    [switch]$C,
    [parameter(Mandatory=$true,ParameterSetName="SET3")]
    [switch]$K
)

$texto = (Get-Content $filePath)
