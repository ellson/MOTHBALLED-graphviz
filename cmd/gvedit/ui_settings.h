/********************************************************************************
** Form generated from reading UI file 'settings.ui'
**
** Created: Mon Feb 21 14:54:23 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QAction *actionCSettingsOK;
    QFrame *frame;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_5;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *cbLayout;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QComboBox *cbExtension;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *leOutput;
    QPushButton *pbOut;
    QHBoxLayout *horizontalLayout_4;
    QHBoxLayout *horizontalLayout_11;
    QSpacerItem *horizontalSpacer_4;
    QCheckBox *chbPreview;
    QSpacerItem *horizontalSpacer_6;
    QCheckBox *chbCairo;
    QSpacerItem *horizontalSpacer_5;
    QFrame *frame_2;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_7;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QComboBox *cbScope;
    QLabel *label_6;
    QComboBox *cbName;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_7;
    QLineEdit *leValue;
    QVBoxLayout *verticalLayout_3;
    QPushButton *pbAdd;
    QSpacerItem *verticalSpacer;
    QPushButton *pushButton_4;
    QFrame *frame_3;
    QWidget *layoutWidget2;
    QVBoxLayout *verticalLayout_4;
    QTextEdit *teAttributes;
    QHBoxLayout *horizontalLayout_10;
    QHBoxLayout *horizontalLayout_8;
    QPushButton *pbNew;
    QPushButton *pbOpen;
    QPushButton *pbSave;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_9;
    QPushButton *pbCancel;
    QPushButton *btnOK;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(405, 512);
        actionCSettingsOK = new QAction(Dialog);
        actionCSettingsOK->setObjectName(QString::fromUtf8("actionCSettingsOK"));
        frame = new QFrame(Dialog);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 401, 181));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Sunken);
        frame->setLineWidth(2);
        layoutWidget = new QWidget(frame);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 381, 161));
        verticalLayout_5 = new QVBoxLayout(layoutWidget);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(layoutWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMinimumSize(QSize(120, 0));
        label->setMargin(5);

        horizontalLayout->addWidget(label);

        cbLayout = new QComboBox(layoutWidget);
        cbLayout->setObjectName(QString::fromUtf8("cbLayout"));
        cbLayout->setMinimumSize(QSize(150, 0));

        horizontalLayout->addWidget(cbLayout);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setMinimumSize(QSize(120, 0));
        label_3->setMargin(5);

        horizontalLayout_3->addWidget(label_3);

        cbExtension = new QComboBox(layoutWidget);
        cbExtension->setObjectName(QString::fromUtf8("cbExtension"));
        cbExtension->setMinimumSize(QSize(150, 0));

        horizontalLayout_3->addWidget(cbExtension);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setMinimumSize(QSize(120, 0));
        label_2->setMargin(5);

        horizontalLayout_2->addWidget(label_2);

        leOutput = new QLineEdit(layoutWidget);
        leOutput->setObjectName(QString::fromUtf8("leOutput"));
        leOutput->setReadOnly(true);

        horizontalLayout_2->addWidget(leOutput);

        pbOut = new QPushButton(layoutWidget);
        pbOut->setObjectName(QString::fromUtf8("pbOut"));
        pbOut->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_2->addWidget(pbOut);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));

        verticalLayout->addLayout(horizontalLayout_4);


        verticalLayout_5->addLayout(verticalLayout);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_4);

        chbPreview = new QCheckBox(layoutWidget);
        chbPreview->setObjectName(QString::fromUtf8("chbPreview"));

        horizontalLayout_11->addWidget(chbPreview);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_6);

        chbCairo = new QCheckBox(layoutWidget);
        chbCairo->setObjectName(QString::fromUtf8("chbCairo"));

        horizontalLayout_11->addWidget(chbCairo);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_5);


        verticalLayout_5->addLayout(horizontalLayout_11);

        frame_2 = new QFrame(Dialog);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setGeometry(QRect(0, 180, 401, 71));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        layoutWidget1 = new QWidget(frame_2);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(10, 0, 381, 62));
        horizontalLayout_7 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_5 = new QLabel(layoutWidget1);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_5->addWidget(label_5);

        cbScope = new QComboBox(layoutWidget1);
        cbScope->setObjectName(QString::fromUtf8("cbScope"));

        horizontalLayout_5->addWidget(cbScope);

        label_6 = new QLabel(layoutWidget1);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        horizontalLayout_5->addWidget(label_6);

        cbName = new QComboBox(layoutWidget1);
        cbName->setObjectName(QString::fromUtf8("cbName"));

        horizontalLayout_5->addWidget(cbName);

        horizontalLayout_5->setStretch(0, 1);
        horizontalLayout_5->setStretch(1, 2);
        horizontalLayout_5->setStretch(2, 1);
        horizontalLayout_5->setStretch(3, 2);

        verticalLayout_2->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_7 = new QLabel(layoutWidget1);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setMinimumSize(QSize(40, 0));
        label_7->setMaximumSize(QSize(47, 16777215));

        horizontalLayout_6->addWidget(label_7);

        leValue = new QLineEdit(layoutWidget1);
        leValue->setObjectName(QString::fromUtf8("leValue"));

        horizontalLayout_6->addWidget(leValue);


        verticalLayout_2->addLayout(horizontalLayout_6);


        horizontalLayout_7->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        pbAdd = new QPushButton(layoutWidget1);
        pbAdd->setObjectName(QString::fromUtf8("pbAdd"));
        pbAdd->setMinimumSize(QSize(75, 0));

        verticalLayout_3->addWidget(pbAdd);

        verticalSpacer = new QSpacerItem(20, 13, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        pushButton_4 = new QPushButton(layoutWidget1);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setMinimumSize(QSize(75, 0));

        verticalLayout_3->addWidget(pushButton_4);


        horizontalLayout_7->addLayout(verticalLayout_3);

        frame_3 = new QFrame(Dialog);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setGeometry(QRect(0, 250, 401, 261));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        layoutWidget2 = new QWidget(frame_3);
        layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(7, 6, 391, 251));
        verticalLayout_4 = new QVBoxLayout(layoutWidget2);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        teAttributes = new QTextEdit(layoutWidget2);
        teAttributes->setObjectName(QString::fromUtf8("teAttributes"));

        verticalLayout_4->addWidget(teAttributes);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        pbNew = new QPushButton(layoutWidget2);
        pbNew->setObjectName(QString::fromUtf8("pbNew"));
        pbNew->setMinimumSize(QSize(40, 0));
        pbNew->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_8->addWidget(pbNew);

        pbOpen = new QPushButton(layoutWidget2);
        pbOpen->setObjectName(QString::fromUtf8("pbOpen"));
        pbOpen->setMinimumSize(QSize(40, 0));
        pbOpen->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_8->addWidget(pbOpen);

        pbSave = new QPushButton(layoutWidget2);
        pbSave->setObjectName(QString::fromUtf8("pbSave"));
        pbSave->setMinimumSize(QSize(40, 0));
        pbSave->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_8->addWidget(pbSave);


        horizontalLayout_10->addLayout(horizontalLayout_8);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_3);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        pbCancel = new QPushButton(layoutWidget2);
        pbCancel->setObjectName(QString::fromUtf8("pbCancel"));

        horizontalLayout_9->addWidget(pbCancel);

        btnOK = new QPushButton(layoutWidget2);
        btnOK->setObjectName(QString::fromUtf8("btnOK"));

        horizontalLayout_9->addWidget(btnOK);


        horizontalLayout_10->addLayout(horizontalLayout_9);


        verticalLayout_4->addLayout(horizontalLayout_10);


        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", 0, QApplication::UnicodeUTF8));
        actionCSettingsOK->setText(QApplication::translate("Dialog", "CSettingsOK", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Dialog", "Layout Engine", 0, QApplication::UnicodeUTF8));
        cbLayout->clear();
        cbLayout->insertItems(0, QStringList()
         << QApplication::translate("Dialog", "dot", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "circo", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "fdp", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "neato", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "nop", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "nop1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "nop2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "osage", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "patchwork", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "sfdp", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "twopi", 0, QApplication::UnicodeUTF8)
        );
        label_3->setText(QApplication::translate("Dialog", "Output file Type", 0, QApplication::UnicodeUTF8));
        cbExtension->clear();
        cbExtension->insertItems(0, QStringList()
         << QApplication::translate("Dialog", "png", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "jpg", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "ps", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "dot", 0, QApplication::UnicodeUTF8)
        );
        label_2->setText(QApplication::translate("Dialog", "Output File Name", 0, QApplication::UnicodeUTF8));
        pbOut->setText(QApplication::translate("Dialog", "...", 0, QApplication::UnicodeUTF8));
        chbPreview->setText(QApplication::translate("Dialog", "Preview Output File", 0, QApplication::UnicodeUTF8));
        chbCairo->setText(QApplication::translate("Dialog", "Apply Cairo Filters", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Dialog", "Scope", 0, QApplication::UnicodeUTF8));
        cbScope->clear();
        cbScope->insertItems(0, QStringList()
         << QApplication::translate("Dialog", "graph", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "node", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "edge", 0, QApplication::UnicodeUTF8)
        );
        label_6->setText(QApplication::translate("Dialog", "Name", 0, QApplication::UnicodeUTF8));
        cbName->clear();
        cbName->insertItems(0, QStringList()
         << QApplication::translate("Dialog", "color", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "Font", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "fontname", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "fontcolor", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Dialog", "bgcolor", 0, QApplication::UnicodeUTF8)
        );
        label_7->setText(QApplication::translate("Dialog", "Value", 0, QApplication::UnicodeUTF8));
        pbAdd->setText(QApplication::translate("Dialog", "Add", 0, QApplication::UnicodeUTF8));
        pushButton_4->setText(QApplication::translate("Dialog", "Help", 0, QApplication::UnicodeUTF8));
        pbNew->setText(QApplication::translate("Dialog", "clear", 0, QApplication::UnicodeUTF8));
        pbOpen->setText(QApplication::translate("Dialog", "load", 0, QApplication::UnicodeUTF8));
        pbSave->setText(QApplication::translate("Dialog", "save", 0, QApplication::UnicodeUTF8));
        pbCancel->setText(QApplication::translate("Dialog", "Cancel", 0, QApplication::UnicodeUTF8));
        btnOK->setText(QApplication::translate("Dialog", "OK", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_H
