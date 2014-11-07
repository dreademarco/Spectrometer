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
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "spectrogramplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QProgressBar *producerProgressBar;
    QLabel *label_2;
    QProgressBar *consumerProgressBar;
    QLabel *label_3;
    QProgressBar *bufferProgressBar;
    QPushButton *startPushButton;
    SpectrogramPlot *plotWidget;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(682, 510);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        producerProgressBar = new QProgressBar(centralWidget);
        producerProgressBar->setObjectName(QStringLiteral("producerProgressBar"));
        producerProgressBar->setValue(24);

        verticalLayout->addWidget(producerProgressBar);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout->addWidget(label_2);

        consumerProgressBar = new QProgressBar(centralWidget);
        consumerProgressBar->setObjectName(QStringLiteral("consumerProgressBar"));
        consumerProgressBar->setValue(24);

        verticalLayout->addWidget(consumerProgressBar);

        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout->addWidget(label_3);

        bufferProgressBar = new QProgressBar(centralWidget);
        bufferProgressBar->setObjectName(QStringLiteral("bufferProgressBar"));
        bufferProgressBar->setValue(24);

        verticalLayout->addWidget(bufferProgressBar);

        startPushButton = new QPushButton(centralWidget);
        startPushButton->setObjectName(QStringLiteral("startPushButton"));

        verticalLayout->addWidget(startPushButton);

        plotWidget = new SpectrogramPlot(centralWidget);
        plotWidget->setObjectName(QStringLiteral("plotWidget"));

        verticalLayout->addWidget(plotWidget);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        label->setText(QApplication::translate("MainWindow", "Producer", 0));
        label_2->setText(QApplication::translate("MainWindow", "Consumer", 0));
        label_3->setText(QApplication::translate("MainWindow", "Buffer", 0));
        startPushButton->setText(QApplication::translate("MainWindow", "Start", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
