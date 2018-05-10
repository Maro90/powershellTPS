<#
.SYNOPSIS
Actualiza un xml de inventario.

.DESCRIPTION
Actualiza un inventario, basandose en 2 archivos, uno de stock  y otro de precios, proporcionados por el proveedor.
    
.PARAMETER pathInventario
Path del archivo de palabras. [OBLIGATORIO].
    
.PARAMETER pathProveedores
Directorio donde se encuentran los archivos de los proveedores. [OBLIGATORIO].
    
.NOTES

Nota: Se han modificado los .xml, quitando los acentos, para poder utilizar el comando .Save()

Nombre del Script: Ejercicio4.ps1

Trabajo Práctico Nro. 1 - Ejercicio 4

Integrantes:
            Arana, Juan Pablo        33904497
            Gonzalez, Mauro Daniel   35368160
            Sapaya, Nicolás Martín   38319489

Instancia de Entrega: Primer Re Entrega
#>

Param([Parameter(Mandatory=$true)][string][validateNotNullorEmpty()]$pathInventario,
      [Parameter(Mandatory=$true)][string][validateNotNullorEmpty()]$pathProveedores,
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

    #Si estaba completamente vacio creo un XML nuevo con su raiz, sino uso existente
    if(-not ($inventario.InnerXml)){
        [xml]$inventario = New-Object System.Xml.XmlDocument
        $root = $inventario.CreateNode("element","inventario",$null)
    }

    #Voy creando los hijos y le asigno los valores
    $newXmlProductElement = $inventario.CreateElement("producto");
    #Si estaba completamente vacio le agrego el producto a la raiz que cree, sino a la existente
    if(-not ($inventario.InnerXml)){

        $newXmlProduct = $root.AppendChild($newXmlProductElement)
    }else{

        $newXmlProduct = $inventario.ChildNodes.AppendChild($newXmlProductElement)
    }

    $newXmlCodigoElement = $newXmlProduct.AppendChild($inventario.CreateElement("codigo"))
    $newXmlCodigoTextNode = $newXmlCodigoElement.AppendChild($inventario.CreateTextNode($codigo))
 
    $newXmlDescripcionElement = $newXmlProduct.AppendChild($inventario.CreateElement("descripcion"))
    $newXmlDescripcionTextNode = $newXmlDescripcionElement.AppendChild($inventario.CreateTextNode($descripcion))

    $newXmlCodigoProvElement = $newXmlProduct.AppendChild($inventario.CreateElement("codigoProveedor"))
    $newXmlCodigoProvTextNode = $newXmlCodigoProvElement.AppendChild($inventario.CreateTextNode($codigoProveedor))
 
    $newXmlPrecioElement = $newXmlProduct.AppendChild($inventario.CreateElement("precio"))
    $newXmlPrecioTextNode = $newXmlPrecioElement.AppendChild($inventario.CreateTextNode($precio))

    $newXmlStockElement = $newXmlProduct.AppendChild($inventario.CreateElement("stock"))
    $newXmlStockTextNode = $newXmlStockElement.AppendChild($inventario.CreateTextNode($stock))

    #Si estaba completamente vacio añado la raiz
    if(-not ($inventario.InnerXml)){
        $inventario.AppendChild($root) | Out-Null
    }

    $inventario.Save($inventarioFullPath)
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
            [Parameter(Mandatory = $true)][validateNotNullorEmpty()]$prodStock,
            [Parameter(Mandatory = $true)][validateNotNullorEmpty()]$invent
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
    $inventario.Save($inventarioFullPath)
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
            [Parameter(Mandatory = $true)][validateNotNullorEmpty()]$prodPrecios,
            [Parameter(Mandatory = $true)][validateNotNullorEmpty()]$invent
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
    $inventario.Save($inventarioFullPath)
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

    .PARAMETER productosInventario
    Productos del inventario.
    #>

    Param (
            [Parameter(Mandatory = $true)][validateNotNullorEmpty()]$producto,
            [Parameter(Mandatory = $true)][validateNotNullorEmpty()]$codProv,
            [Parameter(Mandatory = $true)][validateNotNullorEmpty()]$productosInventario
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

    .PARAMETER productosInventario
    Productos del inventario.
    #>

    Param (
            [Parameter(Mandatory = $true)][validateNotNullorEmpty()]$producto,
            [Parameter(Mandatory = $true)][validateNotNullorEmpty()]$codProv,
            [Parameter(Mandatory = $true)][validateNotNullorEmpty()]$productosInventario
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

    .PARAMETER porcentajeOff
    Indica si devuelve porcentaje o no.
    #>

    Param (
            [Parameter(Mandatory = $true)][validateNotNullorEmpty()]$produ,
            [Parameter(Mandatory = $true)][validateNotNullorEmpty()]$prodPrecio,
            [Parameter(Mandatory = $false)][boolean]$porcentajeOff
    );

    forEach($item in $prodPrecio){
        if($porcentajeOff -eq $true){
            #Si coinciden las descripciones. devuelvo el precio

            #ME TENGO QUE FIJAR SI ES PORCENTAJE O NO

            if($produ -eq $item.ChildNodes.Item(1).'#text'){
                #Si es true, retorno 0 (no me interesa), si es false devuelvo el precio
                if($item.ChildNodes.Item(2).porcentaje){
                    return  "0"
                }else{
                    return [string]($item.ChildNodes.Item(2).'#text')
                }
            }
        }else{
            #Si coinciden las descripciones. devuelvo el precio
            if($produ -eq $item.ChildNodes.Item(1).'#text'){
                return [string]($item.ChildNodes.Item(2).'#text')
            }
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
            [Parameter(Mandatory = $true)][validateNotNullorEmpty()]$stock,
            [Parameter(Mandatory = $true)][validateNotNullorEmpty()]$invent
    );

    #Actualizo Inventario, primero en paro en el primer archivo de stock.
    forEach($item in $stock){

        #Hago GET CONTENT de stock
        [XML]$stockXML = Get-Content ($pathProveedores + '\' + $item.Name)

        #Me fijo si el archivo de stock esta vacio, o solo tiene root
        if(($stockXML.innerXML) -or ($stockXML.stock.innerXML)){

            #Hago GET CONTENT de precio
            [XML]$precioXML = Get-Content ($pathProveedores + '\' + ($item.Name.Replace('stock','precio')))

            #Me fijo si esta vacio el de precios, si es asi no puedo actualizar precios
            if(($precioXML.innerXML) -and ($precioXML.precios.innerXML)){

                #Me fijo si tengo que actualizar precio, si es asi lo hago
                ActualizarPrecio -prodPrecios ($precioXML.precios.producto) -invent $invent
            }

            #Me tengo que fijar si lo que esta en el archivo de stock, esta o no en el de inventario.
            forEach($prod in $stockXML.stock.producto){

                #Me fijo si hay un producto nuevo (debe estar en los dos archivos)[false], o si se tiene que actualizar stock [true]
                if(buscarEnInventario -producto ($prod.ChildNodes.Item(1).'#text') -codProv $prod.ChildNodes.Item(0).'#text' -productosInventario ($invent.inventario.producto)){
                    

                    #Actualizo el stock en el invent
                    ActualizarStock -prodStock $prod -invent $invent
                }else{

                    #Me fijo si esta vacio el de precios, si es asi no puedo insertar nuevos productos
                    if(($precioXML.innerXML) -and ($precioXML.precios.innerXML)){

                        #Primero debo obtener el precio del archivo de precios, luego agrego el producto.
                        [string]$precio = ObtenerPrecio -produ ($prod.ChildNodes.Item(1).'#text') -prodPrecio ($precioXML.precios.producto)
                        AgregarProducto -codigo $prod.ChildNodes.Item(0).'#text' -descripcion $prod.ChildNodes.Item(1).'#text' -codigoProveedor $item.Name.Chars(0) -precio $precio -stock $prod.ChildNodes.Item(2).'#text'
                    }
                }
            }

        }else{

            #Puede estar vacio Stock, pero precios no, entonces debo actualizar precios.
            #Hago GET CONTENT de precio
            [XML]$precioXML = Get-Content ($pathProveedores + '\' + ($item.Name.Replace('stock','precio')))

            #Me fijo si tengo que actualizar precio, si es asi lo hago
            ActualizarPrecio -prodPrecios ($precioXML.precios.producto) -invent $invent
        }
    }
}

function ArchivoVacio{
    <#
    .SYNOPSIS
    Carga los productos validos.

    .DESCRIPTION
    Carga los productos validos, en el invetario que se encontra vacio, o solo con root.
    
    .PARAMETER archivosStock
    Archivos del stock.

    .PARAMETER pathProveedores
    Path a buscar los archivos de proveedores.
    #>

    Param (
            [Parameter(Mandatory = $true)][validateNotNullorEmpty()]$archivosStock,
            [Parameter(Mandatory = $true)]
            [string]$pathProveedores
    );

    forEach($item in $archivosStock){
        
        #Hago GET CONTENT de stock
        [XML]$stockXML = Get-Content ($pathProveedores + '\' + $item.Name)

        #Si esta vacio stock, no hago nada, ya que no debo actualizar precios
        if(($stockXML.innerXML) -or ($stockXML.stock.innerXML)){

            #Hago GET CONTENT de precio
            [XML]$precioXML = Get-Content ($pathProveedores + '\' + ($item.Name.Replace('stock','precio')))

            #Me tengo que fijar si lo que esta en el archivo de stock, esta o no en el de inventario.
            forEach($prod in $stockXML.stock.producto){
            
                #Agrego Producto Si No Tiene Porcentaje En Preci
                [string]$precio = ObtenerPrecio -produ ($prod.ChildNodes.Item(1).'#text') -prodPrecio ($precioXML.precios.producto) -porcentajeOff $true

                if($precio -ne 0){

                    AgregarProducto -codigo $prod.ChildNodes.Item(0).'#text' -descripcion $prod.ChildNodes.Item(1).'#text' -codigoProveedor $item.Name.Chars(0) -precio $precio -stock $prod.ChildNodes.Item(2).'#text'
                    $inventario = [System.Xml.XmlDocument](Get-Content $inventarioFullpath)
                }
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

#Testeo si el/los path/s ingresado/s es/son correcto/s.
if (-not (Test-Path $pathInventario)){
    Write-Error 'El path de Inventario es erroneo. pathInventario, pathProveedores'
    return;
}

if (-not (Test-Path $pathProveedores)){
    Write-Error 'El path de proveedores es erroneo. pathInventario, pathProveedores'
    return;
}

#Guarda la direccion absoluta del documento
$inventarioFullPath = (Get-ChildItem $pathInventario).fullName

#Se realiza back up
if($backUp -eq $true){
    $nameArchivo = ObtainFileName -path $inventarioFullPath
    $content = [System.Xml.XmlDocument] (Get-Content $inventarioFullPath)
    $newName = ChangePath -path $inventarioFullPath -name ($nameArchivo + '_BackUp.xml')
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

#Documento Totalmente Vacio o Productos Vacios
if((-not ($inventario.innerXML)) -or (-not ($inventario.inventario.innerXML))){
   
    ArchivoVacio -archivosStock $archivosStock -pathProveedores $pathProveedores
}else{

    ActualizarInventario -stock $archivosStock -invent $inventario
}