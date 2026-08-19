# CinemaUSAC - Fase 1

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
- Compilador MinGW (GCC, viene con el instalador de Qt / MSYS2)

## Cómo compilar (VS Code + CMake Tools)

1. Abrí la carpeta `EDD_Fase1` en VS Code.
2. `Ctrl+Shift+P` → `CMake: Configure`.
3. Seleccioná el kit **GCC/MinGW** cuando te lo pida.
4. `Ctrl+Shift+P` → `CMake: Build`.
5. Ejecutá el `CinemaUSAC.exe` generado en `build/`.

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