#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);

    PPF *myPPF;
    myPPF = new PPF(16,1024,10);
    return 1;

    //MainWindow w;
    //w.show();

    //return a.exec();
}
