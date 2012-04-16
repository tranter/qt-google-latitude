#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Google API Latitude Client");
    w.show();
    w.startLoginDefault();

    return a.exec();
}
