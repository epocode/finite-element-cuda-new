#include "DialogAddGraphics.h"
#include "MshInformation.h"
#include "publicElement.h"
#include "ui_DialogAddGraphics.h"
#include <gmsh.h>
#include "Controller.h"

DialogAddGraphics::DialogAddGraphics(QWidget *parent, double lc)
    : QDialog(parent)
    , ui(new Ui::DialogAddGraphics)
{
    ui->setupUi(this);
    this->lc = lc;
}

DialogAddGraphics::~DialogAddGraphics()
{
    delete ui;
}

extern MshInformation mshInfo;


void DialogAddGraphics::on_addPointButton_clicked()
{
    QString x = ui->xValue->text();
    QString y = ui->yValue->text();
    QString width = ui->width->text();
    QString height = ui->height->text();
    //创建每个图形对应的curveLoop
    Controller::addRectToMsh(x.toDouble(), y.toDouble(), width.toDouble(), height.toDouble());
    //绘图
    Rect rect;
    rect.x = x.toDouble();
    rect.y = y.toDouble();
    rect.width = width.toDouble();
    rect.height = height.toDouble();
    emit sendRectSignal(rect);
}


void DialogAddGraphics::on_pushButton_clicked()
{
    QString x = ui->circleX->text();
    QString y = ui->circleY->text();
    QString radius = ui->radius->text();
    //添加圆的curveLoop
    Controller::addCircleToMsh(x.toDouble(), y.toDouble(), radius.toDouble());
    //绘图
    Circle circle;
    circle.x = x.toDouble();
    circle.y = y.toDouble();
    circle.radius = radius.toDouble();
    emit sendCircleSignal(circle);
}

