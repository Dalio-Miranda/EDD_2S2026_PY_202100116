cmake_minimum_required(VERSION 3.16)
project(CinemaUSAC VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt genera código a partir de archivos .ui, señales/slots (moc), etc.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Busca el paquete de Qt6 (Widgets para la interfaz gráfica)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Archivos fuente del proyecto
set(SOURCES
    src/main.cpp
)

set(HEADERS
    # Aquí se irán agregando los .h de cada estructura de datos
    # include/Pelicula.h
    # include/ListaDobleEnlazada.h
)

add_executable(CinemaUSAC ${SOURCES} ${HEADERS})

target_include_directories(CinemaUSAC PRIVATE include)

target_link_libraries(CinemaUSAC PRIVATE Qt6::Widgets)
