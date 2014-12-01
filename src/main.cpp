#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);

    PPF *myPPF;
    myPPF = new PPF(4,256,100,1024,50000,2,true);
    delete myPPF;
    return 1;

    //MainWindow w;
    //w.show();

    //return a.exec();
}
