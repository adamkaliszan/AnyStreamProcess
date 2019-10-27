#ifndef DIALOGCONFIG_H
#define DIALOGCONFIG_H

#include <QDialog>

#include "config.h"

namespace Ui {
class DialogConfig;
}

class DialogConfig : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConfig(QWidget *parent = nullptr);
    ~DialogConfig();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DialogConfig *ui;
};

#endif // DIALOGCONFIG_H
