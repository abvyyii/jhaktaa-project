#include <QApplication>
#include <QFont>
#include <QPalette>
#include <QStyleFactory>

#include "mainwindow.h"

namespace {
QPalette buildDarkPalette() {
    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#2D2D30"));
    palette.setColor(QPalette::WindowText, QColor("#F1F1F1"));
    palette.setColor(QPalette::Base, QColor("#252526"));
    palette.setColor(QPalette::AlternateBase, QColor("#2A2A2C"));
    palette.setColor(QPalette::ToolTipBase, QColor("#252526"));
    palette.setColor(QPalette::ToolTipText, QColor("#F1F1F1"));
    palette.setColor(QPalette::Text, QColor("#F1F1F1"));
    palette.setColor(QPalette::Button, QColor("#3C3C3C"));
    palette.setColor(QPalette::ButtonText, QColor("#F1F1F1"));
    palette.setColor(QPalette::BrightText, QColor("#FFFFFF"));
    palette.setColor(QPalette::Link, QColor("#4DA3FF"));
    palette.setColor(QPalette::Highlight, QColor("#007ACC"));
    palette.setColor(QPalette::HighlightedText, QColor("#FFFFFF"));
    palette.setColor(QPalette::Light, QColor("#3E3E42"));
    palette.setColor(QPalette::Midlight, QColor("#323236"));
    palette.setColor(QPalette::Dark, QColor("#202024"));
    palette.setColor(QPalette::Mid, QColor("#3A3A3D"));
    palette.setColor(QPalette::Shadow, QColor("#1B1B1D"));
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor("#7F7F7F"));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#7F7F7F"));
    palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#7F7F7F"));
    return palette;
}
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setStyle(QStyleFactory::create("Fusion"));
    app.setPalette(buildDarkPalette());
    app.setFont(QFont(QStringLiteral("Segoe UI"), 10));

    MainWindow window;
    window.setPalette(buildDarkPalette());
    window.show();
    return app.exec();
}
