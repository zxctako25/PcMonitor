#include "mainwindow.h"

#include <QApplication>
#include <QPalette>
#include <QColor>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setStyle("Fusion");

    QPalette pal;
    pal.setColor(QPalette::Window, QColor(10, 10, 10));
    pal.setColor(QPalette::WindowText, Qt::white);
    pal.setColor(QPalette::Base, QColor(20, 20, 20));
    pal.setColor(QPalette::AlternateBase, QColor(30, 30, 30));
    pal.setColor(QPalette::Text, Qt::white);
    pal.setColor(QPalette::Button, QColor(30, 30, 30));
    pal.setColor(QPalette::ButtonText, Qt::white);
    pal.setColor(QPalette::Highlight, QColor(0, 200, 255));
    pal.setColor(QPalette::HighlightedText, Qt::black);
    QApplication::setPalette(pal);

    QApplication::setWindowIcon(QIcon(":/pcmonitor.ico"));

    MainWindow w;
    w.show();
    return QApplication::exec();
}
