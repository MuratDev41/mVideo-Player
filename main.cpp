#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MOOP::MainWindow w;
    w.show();
    return a.exec();
}
