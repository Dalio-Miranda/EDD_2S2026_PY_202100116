cmake_minimum_required(VERSION 3.16)
project(CinemaUSAC VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt genera código a partir de archivos .ui, señales/slots (moc), etc.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Busca el paquete de Qt6 (Widgets para la interfaz gráfica).
find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/PanelAdministrador.cpp
    src/PanelAdminPeliculas.cpp
    src/PanelAdminPromociones.cpp
    src/PanelAdminSolicitudes.cpp
    src/PanelAdminFunciones.cpp
    src/PanelReportes.cpp
    src/PanelCliente.cpp
    src/PanelClienteCartelera.cpp
    src/PanelClienteAsientos.cpp
    src/PanelClienteCancelarReserva.cpp
    src/PanelClientePromociones.cpp
    src/PanelClienteSolicitudes.cpp
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
    include/PanelReportes.h
    include/PanelCliente.h
    include/PanelClienteCartelera.h
    include/PanelClienteAsientos.h
    include/PanelClienteCancelarReserva.h
    include/PanelClientePromociones.h
    include/PanelClienteSolicitudes.h
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