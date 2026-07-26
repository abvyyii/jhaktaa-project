#include <QApplication>
#include <cstdio>

#include "mainwindow.h"

int main(int argc, char* argv[]) {
    std::fprintf(stderr, "main: creating QApplication\n");
    QApplication app(argc, argv);
    std::fprintf(stderr, "main: creating MainWindow\n");
    MainWindow window;
    std::fprintf(stderr, "main: calling show\n");
    window.show();
    std::fprintf(stderr, "main: entering exec\n");
    const int result = app.exec();
    std::fprintf(stderr, "main: exiting with %d\n", result);
    return result;
}
