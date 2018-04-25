﻿<#
    Nombre del Script: Ejercicio4.ps1

    Trabajo Práctico Nro. 1 - Ejercicio 4

    Integrantes:
                Arana, Juan Pablo        33904497
                Gonzalez, Mauro Daniel   35368160
                Sapaya, Nicolás Martín   38319489

    Instancia de Entrega: Entrega

    Nota: Se han modificado los .xml, quitando los acentos, para poder utilizar el comando .Save()
#>

Param([Parameter(Mandatory=$true)][string]$pathInventario,
      [Parameter(Mandatory=$true)][string]$pathProveedores,
      [switch]$backUp
)

function ObtainFileName {
    <#
    .SYNOPSIS
    Obtiene el nombre del archivo.

    .DESCRIPTION
    Obtiene el nombre del archivo, dandole un path de entrada.
    
    .PARAMETER path
    Path de entrada.
    #>
    Param (
            [Parameter(Mandatory = $true)]
            [string]$path = ''
    );
    
    #Spliteo el path y obtengo el nombre del archivo.
    $splitPath = $path.Split('\') 
    $name = ($splitPath.Get($splitPath.Count-1)).Split('.')
    return $name[0]
}

function AgregarProducto{
    <#
    .SYNOPSIS
    Agrega un item nuevo al inventario.

    .DESCRIPTION
    Agrega un item nuevo al inventario, usando los datos de precios y stock.
    
    .PARAMETER codigo
    Es el codigo del producto.

    .PARAMETER descripcion
    Descrupcion del producto.

    .PARAMETER codigoProveedor
    Es el codigo del proveedor.

    .PARAMETER precio
    Es el precio del producto.

    .PARAMETER stock
    Es el stock del producto.
    #>
    Param (
            [Parameter(Mandatory = $true)][string]$codigo,
            [Parameter(Mandatory = $true)][string]$descripcion,
            [Parameter(Mandatory = $true)][string]$codigoProveedor,
            [Parameter(Mandatory = $true)][string]$precio,
            [Parameter(Mandatory = $true)][string]$stock
    );

    $newXmlProductElement = $inventario.CreateElement("producto");
    $newXmlProduct = $inventario.inventario.AppendChild($newXmlProductElement)

    $newXmlCodigoElement = $newXmlProduct.AppendChild($inventario.CreateElement("codigo"))
    $newXmlCodigoTextNode = $newXmlCodigoElement.AppendChild($inventario.CreateTextNode($codigo))
 
    $newXmlDescripcionElement = $newXmlProduct.AppendChild($inventario.CreateElement("descripcion"))
    $newXmlDescripcionTextNode = $newXmlDescripcionElement.AppendChild($inventario.CreateTextNode($descripcion))

    $newXmlCodigoElement = $newXmlProduct.AppendChild($inventario.CreateElement("codigoProveedor"))
    $newXmlCodigoTextNode = $newXmlCodigoElement.AppendChild($inventario.CreateTextNode($codigoProveedor))
 
    $newXmlDescripcionElement = $newXmlProduct.AppendChild($inventario.CreateElement("precio"))
    $newXmlDescripcionTextNode = $newXmlDescripcionElement.AppendChild($inventario.CreateTextNode($precio))

    $newXmlCodigoElement = $newXmlProduct.AppendChild($inventario.CreateElement("stock"))
    $newXmlCodigoTextNode = $newXmlCodigoElement.AppendChild($inventario.CreateTextNode($stock))

    $inventario.Save($pathInventario)
}

function ActualizarStock{
    <#
    .SYNOPSIS
    Actualiza Stock.

    .DESCRIPTION
    Actualiza el stock, utilizando todos los archivos de stock.
    
    .PARAMETER prodStock
    Productos del stock de un archivo.

    .PARAMETER invent
    Posee el inventario.
    #>
    Param (
            [Parameter(Mandatory = $true)]$prodStock,
            [Parameter(Mandatory = $true)]$invent
    );


    #Itero buscando coincidencias, si las hay, actualizo
    forEach($item in $prodStock){

        #Me fijo si se tiene que actualizar precio [true]
        if(buscarEnInventario -producto ($item.ChildNodes.Item(1).'#text') -codProv $item.ChildNodes.Item(0).'#text' -productosInventario ($invent.inventario.producto)){
            
            #Busco el producto de inventario a escribir
            $invWrite = DevolverProdInv -producto ($item.ChildNodes.Item(1).'#text') -codProv $item.ChildNodes.Item(0).'#text' -productosInventario ($invent.inventario.producto)

            #Sumo Stock
            [int] $stockViejo = $invWrite.ChildNodes.Item(4)."#text"
            [int] $addStock = ($item.ChildNodes.Item(2).'#text')
            [int] $stockNuevo = ($stockViejo + $addStock)
            $invWrite.ChildNodes.Item(4)."#text" = [string] $stockNuevo
        }
    }

    #Guardo cambios
    $inventario.Save($pathInventario)
}

function ActualizarPrecio{
    <#
    .SYNOPSIS
    Actualiza Precio.

    .DESCRIPTION
    Actualiza el precio, utilizando todos los archivos de precios.
    
    .PARAMETER prodPrecios
    Posee los productos de un archivo de precios.

    .PARAMETER invent
    Posee el inventario.
    #>
    Param (
            [Parameter(Mandatory = $true)]$prodPrecios,
            [Parameter(Mandatory = $true)]$invent
    );


    #Itero buscando coincidencias, si las hay, actualizo
    forEach($item in $prodPrecios){
        
        #Me fijo si se tiene que actualizar precio [true]
        if(buscarEnInventario -producto ($item.ChildNodes.Item(1).'#text') -codProv $item.ChildNodes.Item(0).'#text' -productosInventario ($invent.inventario.producto)){
            
            #Busco el producto de inventario a escribir
            $invWrite = DevolverProdInv -producto ($item.ChildNodes.Item(1).'#text') -codProv $item.ChildNodes.Item(0).'#text' -productosInventario ($invent.inventario.producto)

            #[true] es un porcentaje, [false] monto a sobreescribir (actualización de precio)
            if($item.ChildNodes.Item(2).porcentaje){

                [float] $precioViejo = $invWrite.ChildNodes.Item(3)."#text"
                [float] $porcentaje = (($item.ChildNodes.Item(2).'#text') / 100)
                [float] $add = $precioViejo * $porcentaje
                $precioNuevo = ($precioViejo + $add)
                $invWrite.ChildNodes.Item(3)."#text" = [string] $precioNuevo
            }else{

                #Sobreescribo
                $invWrite.ChildNodes.Item(3)."#text" = ($item.ChildNodes.Item(2).'#text')
            }
        }
    }

    #Guardo cambios
    $inventario.Save($pathInventario)
}

function BuscarEnInventario{
    <#
    .SYNOPSIS
    Busca un producto en el inventario.

    .DESCRIPTION
    Busca un producto en el inventario, retorna $false si no encuentra [inserta], $true si encuentra [actualiza].
    
    .PARAMETER producto
    Producto a buscar en el inventario.

    .PARAMETER codProv
    Codigo del proveedor del parametro 'producto'.

    .PARAMETER invent
    Productos del inventario.
    #>

    Param (
            [Parameter(Mandatory = $true)]$producto,
            [Parameter(Mandatory = $true)]$codProv,
            [Parameter(Mandatory = $true)]$productosInventario
    );

    #Itero los productos del inventario para poder comparar
    forEach($item in $productosInventario){

        #Me fijo si coincide la descripcion y el codigo de proveedor (así será el mismo item)
        if(($producto -eq ($item.ChildNodes.Item(1).'#text')) -and ($codProv -eq ($item.ChildNodes.Item(0).'#text'))){
            #Debo Actualizar El Producto (ya existia)
            return $true
        }  
    }

    #Debo Colocar un Nuevo Producto, ya que este no existía
    return $false
}

function DevolverProdInv{
    <#
    .SYNOPSIS
    Devuelve un producto del inventario.

    .DESCRIPTION
    Devuelve un producto del inventario.
    
    .PARAMETER producto
    Producto a buscar en el inventario.

    .PARAMETER codProv
    Codigo del proveedor del parametro 'producto'.

    .PARAMETER invent
    Productos del inventario.
    #>

    Param (
            [Parameter(Mandatory = $true)]$producto,
            [Parameter(Mandatory = $true)]$codProv,
            [Parameter(Mandatory = $true)]$productosInventario
    );

    #Itero los productos del inventario para poder comparar
    forEach($item in $productosInventario){

        #Me fijo si coincide la descripcion y el codigo de proveedor (así será el mismo item)
        if(($producto -eq ($item.ChildNodes.Item(1).'#text')) -and ($codProv -eq ($item.ChildNodes.Item(0).'#text'))){

            #Devuelvo el producto del inventario para actualizar
            return $item
        }  
    }
}

function ObtenerPrecio{
    <#
    .SYNOPSIS
    Obtiene el precio.

    .DESCRIPTION
    Obtiene el precio de un producto nuevo.
    
    .PARAMETER produ
    Producto a buscar el precio.

    .PARAMETER prodPrecio
    Todos los productos del archivo de precios.
    #>

    Param (
            [Parameter(Mandatory = $true)]$produ,
            [Parameter(Mandatory = $true)]$prodPrecio
    );

    forEach($item in $prodPrecio){

        #Si coinciden las descripciones. devuelvo el precio
        if($produ -eq $item.ChildNodes.Item(1).'#text'){
            return [string]($item.ChildNodes.Item(2).'#text')
        }
    }
}

function ActualizarInventario{
    <#
    .SYNOPSIS
    Actualiza el Inventario.

    .DESCRIPTION
    Actualiza el Inventario, utilizando todos los archivos de stock y precios.
    
    .PARAMETER stock
    Todos los archivos de stock.

    .PARAMETER invent
    Posee el inventario.
    #>
    Param (
            [Parameter(Mandatory = $true)]$stock,
            [Parameter(Mandatory = $true)]$invent
    );

    #Actualizo Inventario, primero en paro en el primer archivo de stock.
    forEach($item in $stock){
        
        #Hago GET CONTENT de stock
        [XML]$stockXML = Get-Content ($pathProveedores + '\' + $item.Name)

        #Hago GET CONTENT de precio
        [XML]$precioXML = Get-Content ($pathProveedores + '\' + ($item.Name.Replace('stock','precio')))

        #Me fijo si tengo que actualizar precio, si es asi lo hago
        ActualizarPrecio -prodPrecios ($precioXML.precios.producto) -invent $invent

        #Me tengo que fijar si lo que esta en el archivo de stock, esta o no en el de inventario.
        forEach($prod in $stockXML.stock.producto){

            #Me fijo si hay un producto nuevo (debe estar en los dos archivos)[false], o si se tiene que actualizar stock [true]
            if(buscarEnInventario -producto ($prod.ChildNodes.Item(1).'#text') -codProv $prod.ChildNodes.Item(0).'#text' -productosInventario ($invent.inventario.producto)){
                
                #Actualizo el stock en el inve
                ActualizarStock -prodStock $prod -invent $invent
            }else{

                #Primero debo obtener el precio del archivo de precios, luego agrego el producto.
                [string]$precio = ObtenerPrecio -produ ($prod.ChildNodes.Item(1).'#text') -prodPrecio ($precioXML.precios.producto)
                AgregarProducto -codigo $prod.ChildNodes.Item(0).'#text' -descripcion $prod.ChildNodes.Item(1).'#text' -codigoProveedor $item.Name.Chars(0) -precio $precio -stock $prod.ChildNodes.Item(2).'#text'
            }
        }
    }
}

function ChangePath{
    <#
    .SYNOPSIS
    Cambia la última string del path.

    .DESCRIPTION
    Cambia el path, cambiando la última string por el parametro.
    
    .PARAMETER path
    Path de entrada.

    .PARAMETER name
    Nombre nuevo
    #>
    Param (
            [Parameter(Mandatory = $true)]
            [string]$path = '',
            [Parameter(Mandatory = $true)]
            [string]$name
        );
    
    #Spliteo el path de palabras y creo un nuevo path que será el de puntajes.
    $splitPath = $path.Split('\') 
    $newPath = ''

    #Itero el path spliteado, pero cambio el archivo de palabras por el de puntajes y retorno el path de puntajes.
    forEach($item in $splitPath){
        if( $item -ne $splitPath.Get($splitPath.Count-1)){
            $newPath += $item + '\'
        }else{
            $newPath += $name
        }   
    }
    return $newPath
}


#Se realiza back up
if($backUp -eq $true){
    $nameArchivo = ObtainFileName -path $pathInventario
    $content = [System.Xml.XmlDocument] (Get-Content $pathInventario)
    $newName = ChangePath -path $pathInventario -name ($nameArchivo + '_BackUp.xml')
    $content.Save($newName)
    Write-Output 'Se ha realizado Back Up del archivo Inventario.'
}

#Obtengo todos lo que haya en el directorio de proveedores, luego busco los .xml stock
#Solo busco lo de stock, porque vienen de a pares, si existe el de stock, existe el de precio, accedo cambiando el nombre con replace, en otra funcion
$proveedores = Get-ChildItem $pathProveedores -File -Recurse
forEach($item in $proveedores){

    $archivosStock = ($proveedores | Select-Object -Property Name | Where-Object Name -Match "[0-9]" | Where-Object Name -Match "_" | Where-Object Name -Match ".xml" | Where-Object Name -Match "stock")
}

$inventario = [System.Xml.XmlDocument](Get-Content $pathInventario)

ActualizarInventario -stock $archivosStock -invent $inventario