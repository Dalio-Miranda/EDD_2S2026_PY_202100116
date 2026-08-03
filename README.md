# CinemaUSAC - Fase 1

Proyecto de Estructuras de Datos - USAC. Sistema de gestión de cine implementando
estructuras de datos dinámicas manuales en C++ con interfaz gráfica en Qt6.

## Estructura del proyecto

```
EDD_Fase1/
├── CMakeLists.txt          # Configuración de build
├── src/                    # Archivos .cpp
│   └── main.cpp
├── include/                # Archivos .h (una clase por estructura)
├── data/                   # CSV de prueba para carga masiva
├── reports/                # Salida de los .dot / .png de Graphviz
├── docs/
│   └── documentacion/      # Manual técnico (Markdown o PDF)
└── .gitignore
```

## Requisitos

- Qt 6.x (componente Widgets), instalado con MinGW 64-bit
- CMake 3.16+
- Graphviz instalado y accesible en el PATH (comando `dot`)
- Compilador MinGW (viene con el instalador de Qt)

## Cómo compilar (VS Code + CMake Tools)

1. Abrí la carpeta `EDD_Fase1` en VS Code.
2. `Ctrl+Shift+P` → `CMake: Configure`. Si no detecta Qt automáticamente,
   agregá esto en `.vscode/settings.json`:
   ```json
   {
     "cmake.configureArgs": [
       "-DCMAKE_PREFIX_PATH=C:/Qt/6.7.0/mingw_64"
     ]
   }
   ```
   (ajustá la ruta según donde instalaste Qt).
3. Seleccioná el kit **MinGW** cuando te lo pida.
4. `Ctrl+Shift+P` → `CMake: Build`.
5. Ejecutá el `.exe` generado en `build/`.

## Estado actual

- [x] Esqueleto del proyecto
- [ ] Lista doblemente enlazada (cartelera de películas)
- [ ] Carga masiva CSV
- [ ] Algoritmo de búsqueda por código
- [ ] Lista circular simple (promociones) + lista doble anidada (beneficios)
- [ ] Lista circular doblemente enlazada (solicitudes)
- [ ] Matriz dispersa (mapa de asientos)
- [ ] Reportes Graphviz (4)
- [ ] GUI Qt completa
- [ ] Manual técnico
