#include <QApplication>
#include <QFont>
#include <QIcon>
#include <QImage>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QStyleFactory>

#include "mainwindow.h"

namespace {
QIcon createAppIcon() {
    QPixmap pixmap(QStringLiteral(":/logo.png"));
    if (!pixmap.isNull()) {
        QIcon icon;
        for (const int size : {16, 24, 32, 48, 64, 128, 256}) {
            icon.addPixmap(pixmap.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        return icon;
    }

    QImage image(256, 256, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setBrush(QColor("#0078D7"));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(12, 12, 232, 232);

    painter.setBrush(QColor("#FFFFFF"));
    painter.drawRect(80, 80, 96, 96);

    painter.setPen(QPen(QColor("#FFFFFF"), 12));
    painter.drawLine(80, 128, 176, 128);
    painter.drawLine(128, 80, 128, 176);
    painter.end();

    QIcon icon;
    for (const int size : {16, 24, 32, 48, 64, 128, 256}) {
        QImage scaled = image.scaled(size, size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        icon.addPixmap(QPixmap::fromImage(scaled));
    }
    return icon;
}

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

    const QIcon appIcon = createAppIcon();
    app.setWindowIcon(appIcon);

    MainWindow window;
    window.setWindowIcon(appIcon);
    window.setPalette(buildDarkPalette());
    window.show();
    return app.exec();
}
