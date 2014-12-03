#include "mainwindow.h"
#include "fft_speedtest.h"
#include <QApplication>

int ntaps = 4, nchans = 512, nblocks = 100, srate = 1024, tobs = 10000, nthreads = 2;

void processArgs(int argc, char *argv[])
{
    int i = 1;

    while((fopen(argv[i], "r")) != NULL)
        i++;

    while(i < argc) {
       if (!strcmp(argv[i], "-ntaps"))
           ntaps = atoi(argv[++i]);
       else if (!strcmp(argv[i], "-nchans"))
           nchans = atoi(argv[++i]);
       else if (!strcmp(argv[i], "-nblocks"))
           nblocks = atoi(argv[++i]);
       else if (!strcmp(argv[i], "-srate"))
           srate = atoi(argv[++i]);
       else if (!strcmp(argv[i], "-tobs"))
           tobs = atoi(argv[++i]);
       else if (!strcmp(argv[i], "-nthreads"))
           nthreads = atoi(argv[++i]);
       i++;
    }
}

int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);

    printf("ntaps: %d, nchans: %d, nthreads: %d, tobs: %d, srate; %d\n", ntaps, nchans, nthreads, tobs, srate);
    fft_speedtest *myfft_speedtest;
    myfft_speedtest = new fft_speedtest(ntaps, nchans, srate, tobs, nthreads);
    delete myfft_speedtest;
    return 1;

//    // Process Arguments
//    processArgs(argc, argv);
//    printf("ntaps: %d, nchans: %d, nthreads: %d, tobs: %d, srate; %d\n", ntaps, nchans, nthreads, tobs, srate);
//    PPF *myPPF;
//    myPPF = new PPF(ntaps, nchans, nblocks, srate, tobs, nthreads, true);
//    delete myPPF;
//    return 1;

    //MainWindow w;
    //w.show();

    //return a.exec();
}
