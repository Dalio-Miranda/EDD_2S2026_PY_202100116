# CinemaUSAC - Fase 2

> **Nota:** esta carpeta es una copia completa del proyecto de
> Fase 1, extendida con las estructuras y funcionalidades de Fase 2
> (ver seccion "Fase 2" al final de este documento). Confirmado con
> el auxiliar: solo se califica lo nuevo de Fase 2 aqui adentro; no
> hace falta correr `EDD_Fase1` por separado el dia de la entrega.

Sistema de gestión de cine desarrollado en C++17 con interfaz gráfica en Qt6,
como proyecto del curso de Estructuras de Datos - USAC.

**Estudiante:** Dalio Miranda — Carné 202100116

## Estructura del proyecto

```
EDD_Fase1/
├── CMakeLists.txt          # Configuración de build
├── assets/                 # Recursos graficos (logo USAC, etc.)
├── src/                    # Archivos .cpp
├── include/                # Archivos .h
├── data/                   # CSV de prueba para carga masiva
├── reports/                # Salida de los .dot / .png de Graphviz
├── docs/
│   └── documentacion/      # Manual técnico + capturas
│       ├── manual_tecnico.md
│       └── capturas/
└── .gitignore
```

## Requisitos

- Qt 6.11+ (componente Widgets), instalado con MinGW 64-bit
- CMake 3.16+ con generador Ninja
- Graphviz instalado y accesible en el PATH (comando `dot`)
- El compilador MinGW que trae el propio instalador de Qt
  (`C:/Qt/Tools/mingw1310_64` o equivalente segun tu version) —
  **no mezclar con otro MinGW instalado en la maquina** (ej. MSYS2),
  puede causar errores de enlazado o fallas en tiempo de ejecucion
  por incompatibilidad de ABI entre distintas builds de GCC/MinGW.

## Cómo compilar (VS Code + CMake Tools)

1. Abrí esta carpeta (`EDD_Fase2`) en VS Code.
2. El archivo `.vscode/settings.json` ya trae el compilador de Qt
   forzado explicitamente (via `cmake.configureArgs`), asi que no
   deberia pedirte seleccionar un kit manualmente. Si tu instalacion
   de Qt esta en otra ruta/version, ajusta esas rutas en ese archivo.
3. `Ctrl+Shift+P` → `CMake: Configure`.
4. `Ctrl+Shift+P` → `CMake: Build`.
5. `Ctrl+Shift+P` → `CMake: Run Without Debugging` (Build por si solo
   NO abre la ventana, solo compila). Se genera `CinemaUSAC_Fase2.exe`
   en `build/`.

## Estructuras de datos implementadas (manual, sin STL)

- **Árbol Binario de Búsqueda** — Cartelera de películas, ordenada por código, con recorrido Inorden.
- **Lista Circular de Listas** — Promociones (lista circular simple) con beneficios anidados (lista doblemente enlazada por promoción).
- **Lista Circular Doblemente Enlazada** — Solicitudes especiales de clientes.
- **Matriz Dispersa** (listas ortogonales fila/columna) — Mapa de asientos por función.

## Funcionalidades

### Panel Administrador
- Gestión de películas: agregar, eliminar, carga masiva CSV, alertas de "próximo a retirar"
- Gestión de promociones y sus beneficios
- Gestión de solicitudes: aprobar (en proceso/atendida) o rechazar
- Crear funciones y visualizar/gestionar el mapa de asientos

### Panel Cliente
- Consultar cartelera completa o buscar por código
- Reservar asientos sobre la función activa
- Cancelar reservas propias
- Consultar promociones vigentes según la fecha actual
- Registrar solicitudes especiales y consultar su estado por teléfono

### Reportes Graphviz
Los 4 reportes gráficos (uno por estructura) se generan en tiempo real desde
cada pestaña correspondiente, con botón "Generar Reporte Graphviz".

## Estado del proyecto

- [x] Árbol Binario de Búsqueda (cartelera)
- [x] Lista circular de listas (promociones + beneficios)
- [x] Lista circular doblemente enlazada (solicitudes)
- [x] Matriz dispersa (mapa de asientos)
- [x] Reportes Graphviz (4/4)
- [x] Panel Administrador (4 pestañas)
- [x] Panel Cliente (5 pestañas)
- [x] Interfaz gráfica con estilo personalizado
- [x] Manual técnico con documentación y capturas

## Documentación técnica

Ver [`docs/documentacion/manual_tecnico.md`](docs/documentacion/manual_tecnico.md)
para el detalle de diseño de cada estructura, justificación de algoritmos,
arquitectura de la aplicación y capturas del sistema en funcionamiento.

---

## Fase 2 — estructuras y funcionalidades nuevas

Esta seccion documenta lo agregado sobre la base de Fase 1. El resto
de este README describe lo heredado (sigue existiendo en el codigo,
pero ya no se califica).

### Estructuras nuevas (manual, sin STL)

- **Árbol AVL** (`include/ArbolAVLFunciones.h`) — Funciones de cine,
  clave `codigo_funcion`. Insercion, eliminacion (3 casos), rotaciones
  simples/dobles, busqueda, edicion, recorridos pre/in/post-orden.
  Probado con fuzzing + AddressSanitizer/UBSan.
- **Árbol B de orden 4** (`include/ArbolBClientes.h`) — Clientes,
  clave `id`. Grado minimo t=2. Insercion con division de nodos,
  eliminacion con prestamo/fusion (CLRS 18.3), busqueda, edicion,
  listado inorden. Probado con fuzzing (2000+ operaciones
  intercaladas) + validador de invariantes + ASan/UBSan.
- **Tabla Hash** (`include/TablaHashReservas.h`) — Reservas, clave
  `codigo_reserva`. Encadenamiento separado para colisiones (djb2
  hash). Insercion, eliminacion, busqueda, listado, estadisticas
  (buckets ocupados, colisiones) para el Reporte 4. Probado con
  fuzzing (5000 operaciones intercaladas vs `std::unordered_map` como
  referencia) + ASan/UBSan.
- **GestorAsientosPorFuncion** (`include/GestorAsientosPorFuncion.h`)
  — orquesta el ciclo de vida de la Matriz Dispersa por funcion: la
  carga/vacia segun la funcion activa, persistiendo automaticamente a
  su `F00X_funcion.json` antes de cambiar de funcion y despues de
  cada reserva/cancelacion. NO modifica `MatrizDispersaAsientos` (la
  implementacion interna de Fase 1 se reutiliza intacta, tal como
  pide el enunciado). Probado con archivos reales en disco, incluido
  el caso critico de cambiar de funcion activa sin perder datos, +
  ASan/UBSan.

### Modelos de datos nuevos

- `include/Funcion.h` — con serializacion JSON.
- `include/Cliente.h` — con serializacion JSON (solo guarda
  `codigos_reserva`, no las reservas completas, segun el enunciado).
- `include/Reserva.h` — con serializacion JSON (soporta el formato
  reducido de `F00X_funcion.json`, sin `id_cliente`/`fecha_reserva`).

### Dependencias

- `include/nlohmann/json.hpp` — libreria JSON header-only, incluida
  directamente en el repo (no via FetchContent), para que la
  compilacion no dependa de tener internet el dia de la
  calificacion.

### Pendiente

- Persistencia JSON completa (carga masiva de peliculas+funciones y
  de clientes+reservas) y validaciones de datos.
- Nuevos paneles de UI: gestion de funciones (AVL), gestion de
  clientes (Arbol B), reservar/cancelar/historial de cliente sobre
  las nuevas estructuras (usando `GestorAsientosPorFuncion` para el
  mapa de asientos).
- Reportes Graphviz de las 3 estructuras nuevas (AVL, Arbol B, Tabla
  Hash) mas el reporte especifico de la matriz dispersa por funcion.
- Manual tecnico y manual de usuario de Fase 2 en
  `documentacion/` (carpeta separada del manual de Fase 1 en
  `docs/documentacion/`).