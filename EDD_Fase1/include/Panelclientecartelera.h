#ifndef PANEL_CLIENTE_CARTELERA_H
#define PANEL_CLIENTE_CARTELERA_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include "ArbolBinarioPeliculas.h"

// Pestana de cliente: consultar toda la cartelera o buscar una
// pelicula puntual por codigo usando la busqueda del BST.
class PanelClienteCartelera : public QWidget {
    Q_OBJECT

public:
    explicit PanelClienteCartelera(ArbolBinarioPeliculas& carteleraRef, QWidget* parent = nullptr);

private slots:
    void onBuscar();
    void onVerTodas();

private:
    ArbolBinarioPeliculas& cartelera;
    QLineEdit* campoBusqueda;
    QTableWidget* tabla;

    void mostrarPeliculas(const std::vector<Pelicula>& peliculas);
};

#endif // PANEL_CLIENTE_CARTELERA_H