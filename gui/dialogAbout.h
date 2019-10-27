#ifndef DIALOGABOUT_H
#define DIALOGABOUT_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsItem>

namespace Ui {
    class Dialog;
}

class DialogAbout : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAbout(QWidget *parent = 0);
    ~DialogAbout();

private:
    Ui::Dialog *ui;

    QGraphicsScene *scene;
    QGraphicsEllipseItem *elipse;
};

#endif // DIALOGABOUT_H
