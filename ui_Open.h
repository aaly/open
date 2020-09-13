/********************************************************************************
** Form generated from reading UI file 'Open.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPEN_H
#define UI_OPEN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Open
{
public:
    QWidget *centralWidget;

    void setupUi(QGraphicsView *Open)
    {
        if (Open->objectName().isEmpty())
            Open->setObjectName(QStringLiteral("Open"));
        Open->resize(400, 300);
        centralWidget = new QWidget(Open);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));

        retranslateUi(Open);

        QMetaObject::connectSlotsByName(Open);
    } // setupUi

    void retranslateUi(QGraphicsView *Open)
    {
        Open->setWindowTitle(QApplication::translate("Open", "Open", 0));
    } // retranslateUi

};

namespace Ui {
    class Open: public Ui_Open {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPEN_H
