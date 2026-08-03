#include <QApplication>
#include <QMainWindow>
#include <QLabel>

// Punto de entrada del sistema CinemaUSAC.
// Por ahora solo abre una ventana vacía para confirmar que
// el entorno Qt + CMake está configurado correctamente.
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("CinemaUSAC - Fase 1");
    window.resize(800, 600);

    QLabel *label = new QLabel("Si ves esta ventana, Qt y CMake ya están funcionando.", &window);
    label->setAlignment(Qt::AlignCenter);
    window.setCentralWidget(label);

    window.show();

    return app.exec();
}
