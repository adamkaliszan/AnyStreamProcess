/********************************************************************************
** Form generated from reading UI file 'dialogconfig.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGCONFIG_H
#define UI_DIALOGCONFIG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_DialogConfig
{
public:
    QGridLayout *gridLayout;
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEditDB_userName;
    QLabel *labelDBuserName;
    QLabel *labelDB_URL;
    QLineEdit *lineEditDB_password;
    QLabel *labelDB_password;
    QLineEdit *lineEditDB_URL;
    QLabel *labelDB_name;
    QLineEdit *lineEditDB_name;

    void setupUi(QDialog *DialogConfig)
    {
        if (DialogConfig->objectName().isEmpty())
            DialogConfig->setObjectName(QStringLiteral("DialogConfig"));
        DialogConfig->resize(400, 300);
        gridLayout = new QGridLayout(DialogConfig);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        buttonBox = new QDialogButtonBox(DialogConfig);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 4, 1, 1, 1);

        lineEditDB_userName = new QLineEdit(DialogConfig);
        lineEditDB_userName->setObjectName(QStringLiteral("lineEditDB_userName"));

        gridLayout->addWidget(lineEditDB_userName, 2, 1, 1, 1);

        labelDBuserName = new QLabel(DialogConfig);
        labelDBuserName->setObjectName(QStringLiteral("labelDBuserName"));

        gridLayout->addWidget(labelDBuserName, 2, 0, 1, 1);

        labelDB_URL = new QLabel(DialogConfig);
        labelDB_URL->setObjectName(QStringLiteral("labelDB_URL"));

        gridLayout->addWidget(labelDB_URL, 0, 0, 1, 1);

        lineEditDB_password = new QLineEdit(DialogConfig);
        lineEditDB_password->setObjectName(QStringLiteral("lineEditDB_password"));
        lineEditDB_password->setInputMethodHints(Qt::ImhHiddenText|Qt::ImhNoAutoUppercase|Qt::ImhNoPredictiveText|Qt::ImhSensitiveData);
        lineEditDB_password->setEchoMode(QLineEdit::Password);

        gridLayout->addWidget(lineEditDB_password, 3, 1, 1, 1);

        labelDB_password = new QLabel(DialogConfig);
        labelDB_password->setObjectName(QStringLiteral("labelDB_password"));

        gridLayout->addWidget(labelDB_password, 3, 0, 1, 1);

        lineEditDB_URL = new QLineEdit(DialogConfig);
        lineEditDB_URL->setObjectName(QStringLiteral("lineEditDB_URL"));

        gridLayout->addWidget(lineEditDB_URL, 0, 1, 1, 1);

        labelDB_name = new QLabel(DialogConfig);
        labelDB_name->setObjectName(QStringLiteral("labelDB_name"));

        gridLayout->addWidget(labelDB_name, 1, 0, 1, 1);

        lineEditDB_name = new QLineEdit(DialogConfig);
        lineEditDB_name->setObjectName(QStringLiteral("lineEditDB_name"));

        gridLayout->addWidget(lineEditDB_name, 1, 1, 1, 1);


        retranslateUi(DialogConfig);
        QObject::connect(buttonBox, SIGNAL(accepted()), DialogConfig, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DialogConfig, SLOT(reject()));

        QMetaObject::connectSlotsByName(DialogConfig);
    } // setupUi

    void retranslateUi(QDialog *DialogConfig)
    {
        DialogConfig->setWindowTitle(QApplication::translate("DialogConfig", "Dialog", nullptr));
        lineEditDB_userName->setText(QApplication::translate("DialogConfig", " makgywer_stream", nullptr));
        labelDBuserName->setText(QApplication::translate("DialogConfig", "User Name", nullptr));
        labelDB_URL->setText(QApplication::translate("DialogConfig", "DB URL", nullptr));
        labelDB_password->setText(QApplication::translate("DialogConfig", "Password", nullptr));
        lineEditDB_URL->setText(QApplication::translate("DialogConfig", "mysql-475253.vipserv.org/makgywer_stream", nullptr));
        labelDB_name->setText(QApplication::translate("DialogConfig", "DB name", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DialogConfig: public Ui_DialogConfig {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGCONFIG_H
