#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);

    PPF *myPPF;
    myPPF = new PPF(4,256,300,1024,10000,2,false);
    delete myPPF;
    return 1;

    //MainWindow w;
    //w.show();

    //return a.exec();
}
