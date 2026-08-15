cmake_minimum_required(VERSION 3.16)
project(CinemaUSAC VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt genera código a partir de archivos .ui, señales/slots (moc), etc.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Busca el paquete de Qt6 (Widgets para la interfaz gráfica).
# Por ahora la prueba de la lista de películas corre en consola,
# pero dejamos Qt enganchado para cuando conectemos la GUI.
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Archivos fuente del proyecto
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/PanelAdministrador.cpp
    src/PanelAdminPeliculas.cpp
    src/PanelAdminPromociones.cpp
    src/PanelAdminSolicitudes.cpp
    src/PanelAdminFunciones.cpp
    src/ArbolBinarioPeliculas.cpp
    src/ListaDobleBeneficios.cpp
    src/ListaCircularPromociones.cpp
    src/ListaCircularSolicitudes.cpp
    src/MatrizDispersaAsientos.cpp
    src/ReportesGraphviz.cpp
)

set(HEADERS
    include/MainWindow.h
    include/PanelAdministrador.h
    include/PanelAdminPeliculas.h
    include/PanelAdminPromociones.h
    include/PanelAdminSolicitudes.h
    include/PanelAdminFunciones.h
    include/Pelicula.h
    include/ArbolBinarioPeliculas.h
    include/Beneficio.h
    include/ListaDobleBeneficios.h
    include/Promocion.h
    include/ListaCircularPromociones.h
    include/Solicitud.h
    include/ListaCircularSolicitudes.h
    include/MatrizDispersaAsientos.h
    include/ReportesGraphviz.h
)

add_executable(CinemaUSAC ${SOURCES} ${HEADERS})

target_include_directories(CinemaUSAC PRIVATE include)

target_link_libraries(CinemaUSAC PRIVATE Qt6::Widgets)