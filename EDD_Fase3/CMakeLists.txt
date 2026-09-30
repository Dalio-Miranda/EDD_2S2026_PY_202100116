cmake_minimum_required(VERSION 3.16)
project(CinemaUSAC_Fase3 VERSION 3.0 LANGUAGES CXX)

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

    # --- Estructuras y modelos nuevos de Fase 2 ---
    src/Funcion.cpp
    src/Cliente.cpp
    src/Reserva.cpp
    src/ArbolAVLFunciones.cpp
    src/ArbolBClientes.cpp
    src/TablaHashReservas.cpp
    src/GestorAsientosPorFuncion.cpp
    src/PersistenciaJSON.cpp

    # --- Estructuras nuevas de Fase 3 ---
    src/Sede.cpp
    src/GrafoSedes.cpp
    src/SHA256.cpp
    src/Transaccion.cpp
    src/Bloque.cpp
    src/ArbolMerkle.cpp
    src/Blockchain.cpp

    # --- Paneles de UI nuevos de Fase 2 ---
    src/PanelAdminFunciones2.cpp
    src/PanelAdminClientes.cpp
    src/PanelClienteReservar2.cpp
    src/PanelClienteHistorial2.cpp
    src/PanelClienteCancelarReserva2.cpp
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

    # --- Estructuras y modelos nuevos de Fase 2 ---
    include/Funcion.h
    include/Cliente.h
    include/Reserva.h
    include/ArbolAVLFunciones.h
    include/ArbolBClientes.h
    include/TablaHashReservas.h
    include/GestorAsientosPorFuncion.h
    include/PersistenciaJSON.h

    # --- Estructuras nuevas de Fase 3 ---
    include/Sede.h
    include/GrafoSedes.h
    include/SHA256.h
    include/Transaccion.h
    include/Bloque.h
    include/ArbolMerkle.h
    include/Blockchain.h
    include/ArregloDinamico.h
    include/TipoRecorrido.h

    # --- Paneles de UI nuevos de Fase 2 ---
    include/PanelAdminFunciones2.h
    include/PanelAdminClientes.h
    include/PanelClienteReservar2.h
    include/PanelClienteHistorial2.h
    include/PanelClienteCancelarReserva2.h
)

add_executable(CinemaUSAC_Fase3 ${SOURCES} ${HEADERS})

target_include_directories(CinemaUSAC_Fase3 PRIVATE include)

target_link_libraries(CinemaUSAC_Fase3 PRIVATE Qt6::Widgets)