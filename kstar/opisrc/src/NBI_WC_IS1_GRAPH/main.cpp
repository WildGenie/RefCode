#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication::setStyle(new QWindowsStyle);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
