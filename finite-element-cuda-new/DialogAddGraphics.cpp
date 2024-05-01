#include "DialogAddGraphics.h"
#include "MshInformation.h"
#include "publicElement.h"
#include "ui_DialogAddGraphics.h"

DialogAddGraphics::DialogAddGraphics(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogAddGraphics)
{
    ui->setupUi(this);
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
    QString info = "(" + x + ", " + y + ", " + "width: " + width + " height: " + height;
    Coordinate tempCoordinate;
    tempCoordinate.x = x.toDouble();
    tempCoordinate.y = y.toDouble();
    mshInfo.coordinates.push_back(tempCoordinate);
    tempCoordinate.x = x.toDouble() + width.toDouble();
    tempCoordinate.y = y.toDouble();
    mshInfo.coordinates.push_back(tempCoordinate);
    tempCoordinate.x = x.toDouble() + width.toDouble();
    tempCoordinate.y = y.toDouble() + height.toDouble();
    mshInfo.coordinates.push_back(tempCoordinate);
    tempCoordinate.x = x.toDouble();
    tempCoordinate.y = y.toDouble() + height.toDouble();
    mshInfo.coordinates.push_back(tempCoordinate);

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
    mshInfo.circle.x = x.toDouble();
    mshInfo.circle.y = y.toDouble();
    mshInfo.circle.radius = radius.toDouble();
    emit sendCircleSignal(mshInfo.circle);
}

