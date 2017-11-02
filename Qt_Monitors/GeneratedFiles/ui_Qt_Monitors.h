/********************************************************************************
** Form generated from reading UI file 'Qt_Monitors.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QT_MONITORS_H
#define UI_QT_MONITORS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Qt_MonitorsClass
{
public:
    QWidget *centralWidget;
    QPushButton *push_btn;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Qt_MonitorsClass)
    {
        if (Qt_MonitorsClass->objectName().isEmpty())
            Qt_MonitorsClass->setObjectName(QStringLiteral("Qt_MonitorsClass"));
        Qt_MonitorsClass->resize(1960, 854);
        Qt_MonitorsClass->setMinimumSize(QSize(1650, 854));
        centralWidget = new QWidget(Qt_MonitorsClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        push_btn = new QPushButton(centralWidget);
        push_btn->setObjectName(QStringLiteral("push_btn"));
        push_btn->setGeometry(QRect(40, 770, 75, 23));
        Qt_MonitorsClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(Qt_MonitorsClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1960, 23));
        Qt_MonitorsClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(Qt_MonitorsClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        Qt_MonitorsClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(Qt_MonitorsClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        Qt_MonitorsClass->setStatusBar(statusBar);

        retranslateUi(Qt_MonitorsClass);

        QMetaObject::connectSlotsByName(Qt_MonitorsClass);
    } // setupUi

    void retranslateUi(QMainWindow *Qt_MonitorsClass)
    {
        Qt_MonitorsClass->setWindowTitle(QApplication::translate("Qt_MonitorsClass", "Qt_Monitors", Q_NULLPTR));
        push_btn->setText(QApplication::translate("Qt_MonitorsClass", "\347\202\271\345\207\273\345\257\271\350\261\241", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Qt_MonitorsClass: public Ui_Qt_MonitorsClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QT_MONITORS_H
