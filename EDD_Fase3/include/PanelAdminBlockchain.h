#ifndef PANEL_ADMIN_BLOCKCHAIN_H
#define PANEL_ADMIN_BLOCKCHAIN_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QTextEdit>
#include "Blockchain.h"

// Pestana del administrador para la blockchain (enunciado Fase 3,
// "Funciones de Administrador", punto 3):
//   a. Generar nuevo bloque (manualmente; ademas se genera solo cada
//      4 transacciones nuevas)
//   b. Ver reporte de la blockchain completa (Graphviz, verde/rojo)
//   c. Generar arbol de Merkle de un bloque especifico
//   d. Ver ventas totales registradas en la blockchain
class PanelAdminBlockchain : public QWidget {
    Q_OBJECT

public:
    PanelAdminBlockchain(Blockchain& blockchainRef, QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onGenerarBloque();
    void onVerReporteBlockchain();
    void onVerMerkle();
    void onVerVentas();

private:
    Blockchain& blockchain;

    QLabel* labelEstado;
    QLabel* labelResultado;
    QTableWidget* tablaBloques;
    QTextEdit* textoVentas;

    void actualizar();
    void mostrarImagen(const QString& titulo, const QString& ruta);
    static QString abreviar(const std::string& hash);
};

#endif // PANEL_ADMIN_BLOCKCHAIN_H
