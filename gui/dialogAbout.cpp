#include "dialogAbout.h"
#include "ui_dialogAbout.h"


DialogAbout::DialogAbout(QWidget *parent) : QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsViewAbout->setScene(scene);

    QPixmap pix = QPixmap(":/graphics/Img/logo.png");
    scene->addPixmap(pix);

    //QBrush redBrush(Qt::red);
    //QBrush blueBrush(Qt::red);
    //QPen blackPen(Qt::black);

    //scn->addPixmap( pix )

    //elipse = scene->addEllipse(10, 10, 100, 100, blackPen, redBrush);
    //elipse->setFlags(QGraphicsItem::ItemIsMovable);
}

DialogAbout::~DialogAbout()
{
    delete ui;
}
