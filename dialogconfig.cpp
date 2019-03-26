#include "dialogconfig.h"
#include "ui_dialogconfig.h"



DialogConfig::DialogConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConfig)
{
    ui->setupUi(this);

    ui->lineEditDB_URL->setText(appConfig.value("dbURL").value<QString>());
    ui->lineEditDB_name->setText(appConfig.value("dbName").value<QString>());
    ui->lineEditDB_userName->setText(appConfig.value("dbUserName").value<QString>());
    ui->lineEditDB_password->setText(appConfig.value("dbPassword").value<QString>());
}

DialogConfig::~DialogConfig()
{
    delete ui;
}

void DialogConfig::on_buttonBox_accepted()
{
    appConfig.setValue("dbURL", ui->lineEditDB_URL->text());
    appConfig.setValue("dbName", ui->lineEditDB_name->text());
    appConfig.setValue("dbUserName", ui->lineEditDB_userName->text());
    appConfig.setValue("dbPassword", ui->lineEditDB_password->text());
}
