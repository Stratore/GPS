#include "mainwindow.h"

#include <QApplication>

//le main fournie directement par QT

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
