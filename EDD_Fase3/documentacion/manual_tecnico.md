# CinemaUSAC — Manual Técnico (Fase 3)

**Estudiante:** Dalio Antonio Miranda Guerra — **Carné:** 202100116
**Curso:** Estructuras de Datos, 2S2026 — Universidad de San Carlos de Guatemala
**Repositorio:** `EDD_2S2026_PY_202100116` · **Carpeta:** `EDD_Fase3`

---

## 1. Introducción

CinemaUSAC es un sistema de gestión de cine desarrollado en **C++17** con interfaz de escritorio en **Qt 6** y reportes con **Graphviz**. La Fase 3 extiende el sistema de las fases anteriores con:

| Módulo | Qué aporta |
|---|---|
| Grafo no dirigido ponderado | Modela las sedes y qué tan parecida es la cartelera de cada par |
| Lista de adyacencia | Representación interna del grafo |
| Blockchain | Registro inmutable de las compras, con prueba de trabajo |
| Árbol de Merkle | Verifica la integridad de las transacciones de cada bloque |
| SHA-256 | Función hash usada por la blockchain y el árbol de Merkle |
| LZW | Compresión de la persistencia (`sedes.edd`, `peliculas.edd`, `funciones.edd`) |

**Restricción del curso:** todas las estructuras son manuales. No se usan `std::vector`, `std::list`, `std::map` ni arreglos de la STL. Se usa el contenedor propio `ArregloDinamico<T>`. La única librería externa es **nlohmann/json** (autorizada) para leer y escribir JSON.

## 2. Estructura del proyecto

```
EDD_Fase3/
├── CMakeLists.txt
├── assets/                  # logo USAC
├── blocks/                  # blockN.json de la blockchain (se genera)
├── data/                    # JSON de prueba para carga masiva
├── documentacion/           # este manual, el manual de usuario y capturas
├── include/                 # encabezados (.h) y nlohmann/json.hpp
├── reports/                 # .dot y .png de Graphviz (se genera)
└── src/                     # implementaciones (.cpp)
```

Los archivos `sedes.edd`, `peliculas.edd` y `funciones.edd` se crean en la carpeta del proyecto al cerrar el programa (el ejecutable corre desde `build/`, por eso las rutas son `../`). Están en `.gitignore`.

## 3. Grafo de sedes (`GrafoSedes`)

### 3.1 Modelo

- **Vértice:** una sede (`Sede`: código, nombre, dirección, cantidad de salas, teléfono).
- **Arista:** une dos sedes que comparten al menos una película en su cartelera.
- **Peso:** cantidad de **películas distintas** que ambas sedes tienen en común.
- **No dirigido:** cada arista real se guarda en la lista de **ambos** extremos.

### 3.2 Por qué lista de adyacencia

Con un número moderado de sedes y relaciones que **no** existen entre todos los pares, una matriz de adyacencia desperdicia espacio (O(V²)). La lista guarda solo las conexiones reales, con espacio O(V + E).

### 3.3 Implementación manual

```cpp
struct Arista { std::string sedeVecina; int peso; };

struct NodoGrafoSede {
    Sede dato;
    ArregloDinamico<Arista> aristas;   // lista de adyacencia del vértice
};

class GrafoSedes {
    ArregloDinamico<NodoGrafoSede*> vertices;   // punteros: la dirección de cada nodo nunca cambia
    ...
};
```

Los vértices se guardan como **punteros** para que las direcciones no se invaliden cuando el arreglo crece.

### 3.4 Construcción y actualización de aristas

Las aristas **no se guardan en disco**: se derivan de las funciones. Cada vez que una función se **crea, edita o elimina**, o se hace una carga masiva, se llama a `recalcularRelacionesDeSede(codigoSede, funciones)`, que:

1. Obtiene el conjunto de películas distintas de esa sede (recorriendo el árbol AVL de funciones).
2. Lo compara contra el de cada una de las demás sedes.
3. Fija el peso con `establecerPeso(A, B, peso)`: si `peso > 0` crea o actualiza la arista en ambos extremos; si `peso <= 0` la elimina.

### 3.4.1 Editar y eliminar sedes

- `editarSede(codigo, nombre, direccion, salas, telefono)`: actualiza los datos del vértice. El **código no cambia**, porque las funciones lo referencian.
- `eliminarSede(codigo)`: como el grafo es no dirigido, cada vecino guarda una arista de regreso. Primero se quita esa arista de la lista de adyacencia de cada vecino, luego se saca el puntero del arreglo de vértices y se libera el nodo. Complejidad: O(grado de la sede × aristas por vecino) más el corrimiento del arreglo.
- La interfaz, antes de eliminar, borra también las **funciones** de esa sede (árbol AVL y su archivo de asientos), para no dejar funciones apuntando a una sede inexistente. Las aristas entre las demás sedes no cambian, porque dependen de sus propias carteleras.

### 3.5 Camino más corto (BFS)

`caminoMasCorto(origen, destino)` usa **búsqueda en anchura**, porque lo que se pide es el menor número de **saltos** (aristas), no la suma de pesos. Por eso no se usa Dijkstra.

- Complejidad: **O(V + E)**.
- Devuelve la secuencia de códigos de sede, incluyendo origen y destino. Si no existe camino, devuelve un arreglo vacío.

### 3.6 Ranking y sedes similares

- `listarAristasOrdenadas()`: cada par una sola vez, ordenado de **mayor a menor peso**. Alimenta el ranking del administrador.
- `sedesSimilares(codigo)`: vecinos de una sede ordenados por peso descendente. Alimenta el botón "Ver Sedes Similares" del cliente.

## 4. SHA-256 (`SHA256`)

Implementación manual del estándar **FIPS 180-4**: relleno del mensaje (bit `1`, ceros y longitud en 64 bits), 64 constantes `K`, mensaje expandido a 64 palabras y compresión en rondas. Devuelve 64 caracteres hexadecimales en minúscula.

```cpp
static std::string calcular(const std::string& texto);
```

Se verificó contra la herramienta `sha256sum` con varias entradas (vacía, cortas, de más de un bloque).

## 5. Blockchain

### 5.1 Bloque

Cada bloque se guarda como `blocks/blockN.json` con llaves en mayúscula:

| Campo | Descripción |
|---|---|
| `INDEX` | Posición en la cadena |
| `TIMESTAMP` | `DD-MM-AAAA::HH:MM:SS` |
| `NONCE` | Número encontrado en la prueba de trabajo |
| `DATA` | Transacciones del bloque |
| `ROOTMERKLE` | Raíz del árbol de Merkle de `DATA` |
| `PREVIOUSHASH` | Hash del bloque anterior (`"0000"` en el primero) |
| `HASH` | Hash del bloque |

### 5.2 Fórmula del hash

```
HASH = SHA256( INDEX + TIMESTAMP + NONCE + ROOTMERKLE + PREVIOUSHASH )
```

`DATA` **no entra directamente** en la fórmula: entra a través de `ROOTMERKLE`. Por eso, para detectar que una transacción fue alterada hay que **recalcular la raíz de Merkle** desde `DATA` y compararla con `ROOTMERKLE`.

### 5.3 Prueba de trabajo

`minar()` incrementa `NONCE` desde 0 hasta que `HASH` empieza con **4 ceros** (`DIFICULTAD_PRUEBA_TRABAJO = 4`).

### 5.4 Estructura en memoria

La cadena es una **lista enlazada** con inserción por la cabeza: el bloque más nuevo es la cabeza y cada nodo apunta a su anterior. Las transacciones nuevas esperan en una **cola de pendientes** en memoria.

### 5.5 Generación de bloques

- **Automática:** al llegar a **4 transacciones pendientes** se mina un bloque (`TRANSACCIONES_POR_BLOQUE_AUTOMATICO = 4`).
- **Manual:** el administrador puede usar "Generar Nuevo Bloque" con las pendientes que haya.
- **Al cerrar el programa:** si quedan transacciones pendientes (menos de 4), se mina un bloque con ellas antes de guardar, para no perder compras (la Tabla Hash se reconstruye solo desde la blockchain). Esta indicación la dio el auxiliar en el foro.

### 5.6 Verificación de integridad al iniciar

`cargarDesdeDisco()` lee los `blockN.json` del más antiguo al más nuevo y valida cada bloque:

1. Su `PREVIOUSHASH` coincide con el `HASH` real del bloque anterior (o es `"0000"` si es el primero).
2. Su `HASH` guardado coincide con el recalculado por `calcularHash()` (detecta cambios en `INDEX`, `TIMESTAMP`, `NONCE`, `ROOTMERKLE` o `PREVIOUSHASH`).
3. La raíz de Merkle recalculada desde `DATA` coincide con `ROOTMERKLE`.

**Al primer bloque inválido la carga se detiene.** Solo el prefijo válido pasa a memoria. Se muestra una advertencia con el índice del primer bloque manipulado, los bloques válidos cargados y los descartados. El reporte marca los bloques íntegros en **verde** y los manipulados en **rojo**.

### 5.7 Tabla hash de reservas

La Tabla Hash de reservas se **reconstruye únicamente a partir del `DATA` de los bloques válidos**. Una reserva que no esté en un bloque válido no vuelve a existir tras reiniciar.

### 5.8 Consultas sobre la blockchain

- `historialDeCliente`: las transacciones de un cliente.
- `buscarTransaccion`: genera un comprobante a partir del código de transacción.
- `calcularVentasTotales`, `desglosePorFuncion`, `desglosePorFecha`: totales de ventas.

## 6. Árbol de Merkle (`ArbolMerkle`)

- **Hoja** = `SHA256( transacción.aTexto() )`.
- **Nodo interno** = `SHA256( hashIzquierdo + hashDerecho )`.
- Si un nivel tiene cantidad **impar** de nodos, el último se **duplica como un nodo nuevo** (no se comparte el puntero, para evitar doble liberación de memoria).
- La **raíz** se guarda en `ROOTMERKLE` del bloque.

Si cualquier transacción cambia, su hoja cambia, y el cambio sube hasta la raíz.

## 7. Compresión LZW (`LZW`)

### 7.1 Algoritmo

- **Compresión:** se parte de un diccionario con las 256 cadenas de un byte. Se busca la cadena más larga conocida `w`, se emite su código y se agrega `w + siguienteCaracter` al diccionario con un código nuevo.
- **Descompresión:** reconstruye el mismo diccionario sobre la marcha, sin necesitar que se guarde. Maneja el caso especial `cScSc` (un código que todavía no está en el diccionario).

### 7.2 Decisiones de implementación

| Aspecto | Decisión |
|---|---|
| Tamaño de código | **Fijo, 16 bits** (2 bytes, *big-endian*) |
| Capacidad | Hasta 65 536 entradas; al llenarse el diccionario deja de agregar y sigue comprimiendo con lo que tiene |
| Diccionario al comprimir | **Tabla hash manual** (clave: cadena) |
| Diccionario al descomprimir | **Arreglo indexado por código** |
| Contenedores | `ArregloDinamico<unsigned char>` (sin STL) |

Como los códigos son de 2 bytes, un archivo comprimido con tamaño **impar** se considera dañado.

## 8. Persistencia `.edd` (`PersistenciaEDD`)

### 8.1 Flujo de guardado (al cerrar la ventana)

```
Estructura en memoria → recorrido → texto en una variable (std::string) → LZW → bytes → archivo .edd
```

**No existe ningún archivo JSON ni XML intermedio.** El texto estructurado vive solo en memoria y se entrega directamente a `LZW::comprimir`. Se serializa siempre el **estado actual** de las estructuras; no se vuelve a leer el archivo de carga masiva.

| Archivo | Contenido | Recorrido usado |
|---|---|---|
| `sedes.edd` | `{"sedes": [...]}` | Vértices del grafo |
| `peliculas.edd` | `{"peliculas": [...]}` | Preorden del árbol de películas |
| `funciones.edd` | `{"funciones": [...]}` | Preorden del árbol AVL |

Se usa **preorden** para que, al reinsertar en ese orden, el árbol conserve su forma.

Cada función se guarda con su `codigo_pelicula` explícito, porque en la carga masiva la función va anidada dentro de su película y su JSON normal no incluye ese dato. Sin él, al reabrir el programa las funciones quedarían sin película y las aristas del grafo se calcularían mal.

### 8.2 Flujo de carga (al iniciar)

1. Se leen los tres archivos y se descomprimen con LZW.
2. Se parsean **todos** antes de tocar nada: si alguno está dañado, no se modifica ninguna estructura (carga atómica) y el programa arranca vacío.
3. Se reconstruyen en orden: sedes → películas → funciones.
4. Se recalculan las aristas del grafo a partir de las funciones.

## 9. Carga masiva (JSON)

La carga masiva se hace desde la pestaña **Funciones (Fase 2)** con "Cargar JSON (Peliculas+Funciones)".

```json
{
  "sedes":     [ { "codigo", "nombre", "direccion", "cantidad_salas", "telefono" } ],
  "peliculas": [ { "codigo", "titulo", "genero", "duracion", "clasificacion", "idioma",
                   "fecha_estreno", "fecha_fin",
                   "funciones": [ { "codigo_funcion", "fecha", "horario", "sede",
                                    "sala", "filas", "columnas", "archivo_asientos" } ] } ]
}
```

**Orden de procesamiento:** primero las **sedes** (para poder validar la `sede` de cada función), luego películas con sus funciones, y al final se recalcula el grafo.

**Validaciones:** campos obligatorios, tipos de dato, fechas `AAAA-MM-DD` válidas, duración y dimensiones enteras positivas, códigos duplicados, y que la `sede` de cada función exista. Un registro inválido se rechaza con su motivo y **no detiene** el resto de la carga; si se rechaza una película, también se rechazan sus funciones.

Archivos de prueba incluidos: `data/peliculas_fase3_ejemplo.json` y `data/peliculas_fase3_completo.json` (este último incluye registros inválidos a propósito).

## 10. Reportes Graphviz

Todos los reportes se generan con `dot` y se muestran dentro de la aplicación, en la pestaña **Reportes** (carrusel de 12).

| N.º | Reporte | Archivo |
|---|---|---|
| 9 | Grafo no dirigido de sedes | `reporte_grafo_sedes.png` |
| 10 | Lista de adyacencia | `reporte_lista_adyacencia.png` |
| 11 | Blockchain (verde = íntegro, rojo = manipulado) | `reporte_blockchain.png` |
| 12 | Árbol de Merkle del último bloque | `reporte_merkle.png` |

El grafo usa `graph` con aristas `--` (no dirigido) y la disposición `circo`. Cada vértice muestra código, nombre, dirección, salas y teléfono; cada arista, su peso.

## 11. Supuestos y decisiones de diseño

- **Precio del boleto:** constante del sistema de **Q 45.00** por boleto (confirmado por el auxiliar en el foro).
- **Ventas totales:** se suma el `DATA` de **todos** los bloques válidos.
- **Reservas canceladas:** el bloque donde quedó la compra **no se modifica** (la blockchain es inmutable). Al cancelar se libera el asiento, se retira la reserva de la tabla hash y se registra una **transacción nueva** con el mismo `codigo_transaccion` y `"estado": "CANCELADA"`, que viaja en un bloque posterior (criterio confirmado por el auxiliar: ej. T003 PAGADA en el bloque 2 y T003 CANCELADA en el bloque 3). Las ventas totales restan las canceladas; al reiniciar, la tabla hash se reconstruye aplicando las cancelaciones en orden. El campo `estado` es opcional al leer JSON (por defecto PAGADA) y solo entra al hash de la hoja de Merkle cuando es CANCELADA.
- **DATA del bloque:** cada bloque guarda únicamente sus propias transacciones (ej. bloque 2 con las 1-8, bloque 3 con las 9-28), tal como lo muestra el diagrama del auxiliar.
- **Credenciales del administrador:** `AdminCine@gmail.com` / `admin123Pass`, definidas en el enunciado.
- **Interfaz:** de escritorio con Qt; la mención a "interfaz web" del enunciado es opcional para esta sección (confirmado con el auxiliar).

## 12. Compilación y ejecución

**Requisitos:** Qt 6.11 (MinGW 64-bit), CMake 3.16+ con Ninja, Graphviz (`dot` en el PATH). Usar el compilador MinGW que trae Qt (`C:/Qt/Tools/mingw1310_64`), sin mezclarlo con otro MinGW.

1. Abrir `EDD_Fase3` en VS Code.
2. `CMake: Configure` y luego `CMake: Build`.
3. En `build/`, copiar las DLL de Qt: `windeployqt --compiler-runtime CinemaUSAC_Fase3.exe`.
4. Ejecutar `CinemaUSAC_Fase3.exe` **desde `build/`** (las rutas `../reports`, `../blocks` y `../assets` dependen de eso).

## 13. Capturas del sistema

![Reporte del grafo de sedes](capturas/09_reporte_grafo.png)

![Reporte de la lista de adyacencia](capturas/10_reporte_lista_adyacencia.png)

![Reporte de la blockchain](capturas/11_reporte_blockchain.png)

![Reporte del árbol de Merkle](capturas/12_reporte_merkle.png)
