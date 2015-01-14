/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "spectrogramplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout_2;
    QComboBox *comboBox_srate;
    QLabel *label_2;
    QComboBox *comboBox_bufferFactor;
    QLabel *label_3;
    QLabel *label;
    QLabel *label_4;
    QComboBox *comboBox_spectSamples;
    QComboBox *comboBox_integrationFactor;
    QComboBox *comboBox_chans;
    QLabel *label_bufsize;
    QLabel *label_6;
    QLabel *label_7;
    QComboBox *comboBox_taps;
    QLabel *label_5;
    QPushButton *startPushButton;
    QRadioButton *stdRadioButton;
    QRadioButton *jetRadioButton;
    QRadioButton *grayRadioButton;
    QPushButton *stopPushButton;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout;
    SpectrogramPlot *plotWidget;
    QWidget *tab_2;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(743, 443);
        MainWindow->setAutoFillBackground(false);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        comboBox_srate = new QComboBox(centralWidget);
        comboBox_srate->setObjectName(QStringLiteral("comboBox_srate"));

        gridLayout_2->addWidget(comboBox_srate, 1, 3, 1, 1);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_2->addWidget(label_2, 0, 2, 1, 1);

        comboBox_bufferFactor = new QComboBox(centralWidget);
        comboBox_bufferFactor->setObjectName(QStringLiteral("comboBox_bufferFactor"));

        gridLayout_2->addWidget(comboBox_bufferFactor, 2, 5, 1, 1);

        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_2->addWidget(label_3, 0, 4, 1, 1);

        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_2->addWidget(label_4, 1, 0, 1, 1);

        comboBox_spectSamples = new QComboBox(centralWidget);
        comboBox_spectSamples->setObjectName(QStringLiteral("comboBox_spectSamples"));

        gridLayout_2->addWidget(comboBox_spectSamples, 1, 1, 1, 1);

        comboBox_integrationFactor = new QComboBox(centralWidget);
        comboBox_integrationFactor->setObjectName(QStringLiteral("comboBox_integrationFactor"));

        gridLayout_2->addWidget(comboBox_integrationFactor, 0, 5, 1, 1);

        comboBox_chans = new QComboBox(centralWidget);
        comboBox_chans->setObjectName(QStringLiteral("comboBox_chans"));

        gridLayout_2->addWidget(comboBox_chans, 0, 3, 1, 1);

        label_bufsize = new QLabel(centralWidget);
        label_bufsize->setObjectName(QStringLiteral("label_bufsize"));

        gridLayout_2->addWidget(label_bufsize, 1, 5, 1, 1);

        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout_2->addWidget(label_6, 1, 4, 1, 1);

        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout_2->addWidget(label_7, 2, 4, 1, 1);

        comboBox_taps = new QComboBox(centralWidget);
        comboBox_taps->setObjectName(QStringLiteral("comboBox_taps"));

        gridLayout_2->addWidget(comboBox_taps, 0, 1, 1, 1);

        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout_2->addWidget(label_5, 1, 2, 1, 1);

        startPushButton = new QPushButton(centralWidget);
        startPushButton->setObjectName(QStringLiteral("startPushButton"));
        startPushButton->setEnabled(true);

        gridLayout_2->addWidget(startPushButton, 3, 0, 1, 1);

        stdRadioButton = new QRadioButton(centralWidget);
        stdRadioButton->setObjectName(QStringLiteral("stdRadioButton"));

        gridLayout_2->addWidget(stdRadioButton, 3, 3, 1, 1);

        jetRadioButton = new QRadioButton(centralWidget);
        jetRadioButton->setObjectName(QStringLiteral("jetRadioButton"));
        jetRadioButton->setChecked(true);

        gridLayout_2->addWidget(jetRadioButton, 3, 4, 1, 1);

        grayRadioButton = new QRadioButton(centralWidget);
        grayRadioButton->setObjectName(QStringLiteral("grayRadioButton"));

        gridLayout_2->addWidget(grayRadioButton, 3, 5, 1, 1);

        stopPushButton = new QPushButton(centralWidget);
        stopPushButton->setObjectName(QStringLiteral("stopPushButton"));

        gridLayout_2->addWidget(stopPushButton, 3, 1, 1, 1);


        verticalLayout->addLayout(gridLayout_2);

        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        gridLayout = new QGridLayout(tab);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        plotWidget = new SpectrogramPlot(tab);
        plotWidget->setObjectName(QStringLiteral("plotWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(plotWidget->sizePolicy().hasHeightForWidth());
        plotWidget->setSizePolicy(sizePolicy1);
        plotWidget->setFocusPolicy(Qt::TabFocus);
        plotWidget->setAutoFillBackground(true);

        gridLayout->addWidget(plotWidget, 1, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        tabWidget->addTab(tab_2, QString());

        verticalLayout->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "ISSA Spectrometer", 0));
        comboBox_srate->clear();
        comboBox_srate->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "1024", 0)
         << QApplication::translate("MainWindow", "2048", 0)
        );
        label_2->setText(QApplication::translate("MainWindow", "Channels", 0));
        comboBox_bufferFactor->clear();
        comboBox_bufferFactor->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "1", 0)
         << QApplication::translate("MainWindow", "2", 0)
         << QApplication::translate("MainWindow", "3", 0)
         << QApplication::translate("MainWindow", "4", 0)
         << QApplication::translate("MainWindow", "5", 0)
        );
        label_3->setText(QApplication::translate("MainWindow", "Integration Factor", 0));
        label->setText(QApplication::translate("MainWindow", "PFB Taps", 0));
        label_4->setText(QApplication::translate("MainWindow", "Spectrogram Size", 0));
        comboBox_spectSamples->clear();
        comboBox_spectSamples->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "64", 0)
         << QApplication::translate("MainWindow", "128", 0)
         << QApplication::translate("MainWindow", "256", 0)
         << QApplication::translate("MainWindow", "512", 0)
         << QApplication::translate("MainWindow", "1024", 0)
         << QApplication::translate("MainWindow", "2048", 0)
         << QApplication::translate("MainWindow", "4096", 0)
         << QApplication::translate("MainWindow", "8192", 0)
         << QApplication::translate("MainWindow", "16384", 0)
        );
        comboBox_integrationFactor->clear();
        comboBox_integrationFactor->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "1", 0)
         << QApplication::translate("MainWindow", "2", 0)
         << QApplication::translate("MainWindow", "4", 0)
         << QApplication::translate("MainWindow", "8", 0)
         << QApplication::translate("MainWindow", "16", 0)
         << QApplication::translate("MainWindow", "32", 0)
         << QApplication::translate("MainWindow", "64", 0)
        );
        comboBox_chans->clear();
        comboBox_chans->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "128", 0)
         << QApplication::translate("MainWindow", "256", 0)
         << QApplication::translate("MainWindow", "512", 0)
         << QApplication::translate("MainWindow", "1024", 0)
        );
        label_bufsize->setText(QApplication::translate("MainWindow", "1", 0));
        label_6->setText(QApplication::translate("MainWindow", "Buffer Unit Duration (s)", 0));
        label_7->setText(QApplication::translate("MainWindow", "Buffer Size Factor", 0));
        comboBox_taps->clear();
        comboBox_taps->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "4", 0)
         << QApplication::translate("MainWindow", "8", 0)
         << QApplication::translate("MainWindow", "16", 0)
         << QApplication::translate("MainWindow", "32", 0)
         << QApplication::translate("MainWindow", "64", 0)
        );
        label_5->setText(QApplication::translate("MainWindow", "Sampling Rate (Hz)", 0));
        startPushButton->setText(QApplication::translate("MainWindow", "Start Plotter", 0));
        stdRadioButton->setText(QApplication::translate("MainWindow", "Standard", 0));
        jetRadioButton->setText(QApplication::translate("MainWindow", "Jet", 0));
        grayRadioButton->setText(QApplication::translate("MainWindow", "Grayscale", 0));
        stopPushButton->setText(QApplication::translate("MainWindow", "Stop Plotter", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Spectrogram", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Bandpass", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
