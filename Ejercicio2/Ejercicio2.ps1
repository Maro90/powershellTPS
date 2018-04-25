<#
    .SYNOPSIS
    Cuenta palabras en titulos de html

    .DESCRIPTION
    Cuenta las palabras del tag title en los html contenidos en al path indicado ignorando las preposiciones
    
    .PARAMETER Prompt
    Path: Direccion donde buscar html (busqueda recursiva)

    .EXAMPLE
    Ejercicio2.ps1 data                                                   
    Paginas analizadas: 3
    Resultados obtenidos:

    # Palabra Cantidad de ocurrencias
    - ------- -----------------------
    1 hola    2                      
    2 chau    2                      
    3 hola2   1  

    .NOTES
    Nombre del Script: Ejercicio2.ps1

    Trabajo Práctico Nro. 1 - Ejercicio 2

    Integrantes:
            Arana, Juan Pablo        33904497
            Gonzalez, Mauro Daniel   35368160
            Sapaya, Nicolás Martín   38319489

    Instancia de Entrega: Entrega
#>

Param(
  [string]$path
  )

$filesCount = 0
$words = @{}
$preps = @("a", "ante", "bajo", "cabe", "con", "contra", "de", "desde", "durante", "en", "entre", "hacia", "hasta", "mediante", "para", "por", "según", "sin", "so", "sobre", "tras", "versus", "vía")

Get-ChildItem -Path $path -Recurse | ?{$_.name.Contains(".html")} | %{
    $filesCount++
    $title = Select-Xml -Path $_.fullname -XPath "/html/head/title" | Select-Object -ExpandProperty Node | Select-Object -ExpandProperty innerxml
    $title.trim().split(" ") | %{
        if(-not $preps.Contains($_))
            {
                $words[$_]++
            }
    }    
}

Write-Host "Paginas analizadas: $filesCount"
Write-Host "Resultados obtenidos:"
$Global:pos=0
$words.GetEnumerator() | sort-object -property value -descending | format-table @{label = "#"; Expression = {$Global:pos; $Global:pos++;}},@{expression="Name";label="Palabra"; alignment="left"}, @{expression="Value"; label="Cantidad de ocurrencias"; alignment="left"}

