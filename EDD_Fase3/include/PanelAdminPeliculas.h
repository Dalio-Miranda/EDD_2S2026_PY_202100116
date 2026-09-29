#ifndef PANEL_ADMIN_PELICULAS_H
#define PANEL_ADMIN_PELICULAS_H

#include <QWidget>
#include <QTableWidget>
#include "ArbolBinarioPeliculas.h"

// Pestana del administrador para gestionar la cartelera de peliculas:
// agregar, eliminar, cargar CSV masivo, ver el estado de cada
// pelicula (con alerta si esta proxima a retirarse) y generar
// el reporte Graphviz del arbol.
class PanelAdminPeliculas : public QWidget {
    Q_OBJECT

public:
    explicit PanelAdminPeliculas(ArbolBinarioPeliculas& carteleraRef, QWidget* parent = nullptr);

private slots:
    void onAgregarPelicula();
    void onEliminarPelicula();
    void onCargarCSV();
    void onGenerarReporte();

private:
    ArbolBinarioPeliculas& cartelera;
    QTableWidget* tabla;

    void actualizarTabla();
    QString calcularEstado(const Pelicula& p) const;
};

#endif // PANEL_ADMIN_PELICULAS_H