#include <QApplication>
#include <QFont>
#include <QStyleFactory>

#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setStyle(QStyleFactory::create("Fusion"));
    app.setFont(QFont(QStringLiteral("Segoe UI"), 10));

    MainWindow window;
    window.show();
    return app.exec();
}
