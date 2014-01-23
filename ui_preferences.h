/********************************************************************************
** Form generated from reading UI file 'preferences.ui'
**
** Created: Tue Nov 27 14:00:17 2012
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREFERENCES_H
#define UI_PREFERENCES_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_Preferences
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QRadioButton *color_inplane;
    QRadioButton *color_x;
    QRadioButton *color_y;
    QRadioButton *color_z;
    QButtonGroup *coloringGroup;

    void setupUi(QDialog *Preferences)
    {
        if (Preferences->objectName().isEmpty())
            Preferences->setObjectName(QString::fromUtf8("Preferences"));
        Preferences->resize(472, 582);
        buttonBox = new QDialogButtonBox(Preferences);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(70, 520, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(Preferences);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 10, 121, 17));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setTextFormat(Qt::AutoText);
        color_inplane = new QRadioButton(Preferences);
        coloringGroup = new QButtonGroup(Preferences);
        coloringGroup->setObjectName(QString::fromUtf8("coloringGroup"));
        coloringGroup->addButton(color_inplane);
        color_inplane->setObjectName(QString::fromUtf8("color_inplane"));
        color_inplane->setGeometry(QRect(30, 30, 131, 22));
        color_inplane->setChecked(true);
        color_x = new QRadioButton(Preferences);
        coloringGroup->addButton(color_x);
        color_x->setObjectName(QString::fromUtf8("color_x"));
        color_x->setGeometry(QRect(30, 50, 116, 22));
        color_y = new QRadioButton(Preferences);
        coloringGroup->addButton(color_y);
        color_y->setObjectName(QString::fromUtf8("color_y"));
        color_y->setGeometry(QRect(30, 70, 116, 22));
        color_z = new QRadioButton(Preferences);
        coloringGroup->addButton(color_z);
        color_z->setObjectName(QString::fromUtf8("color_z"));
        color_z->setGeometry(QRect(30, 90, 116, 22));

        retranslateUi(Preferences);
        QObject::connect(buttonBox, SIGNAL(accepted()), Preferences, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Preferences, SLOT(reject()));

        QMetaObject::connectSlotsByName(Preferences);
    } // setupUi

    void retranslateUi(QDialog *Preferences)
    {
        Preferences->setWindowTitle(QApplication::translate("Preferences", "Muview Preferences", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Preferences", "Coloring Options", 0, QApplication::UnicodeUTF8));
        color_inplane->setText(QApplication::translate("Preferences", "In-Plane Angle", 0, QApplication::UnicodeUTF8));
        color_x->setText(QApplication::translate("Preferences", "X Coordinate", 0, QApplication::UnicodeUTF8));
        color_y->setText(QApplication::translate("Preferences", "Y Coordinate", 0, QApplication::UnicodeUTF8));
        color_z->setText(QApplication::translate("Preferences", "Z Coordinate", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Preferences: public Ui_Preferences {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFERENCES_H
