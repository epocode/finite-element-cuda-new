#include "DialogAddForces.h"
#include "ui_DialogAddForces.h"
#include "publicElement.h"
#include "MshInformation.h"
#include <queue>
#include <QMessageBox>
#include "Controller.h"

extern MshInformation mshInfo;

DialogAddForces::DialogAddForces(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogAddForces)
{
    ui->setupUi(this);
}

DialogAddForces::~DialogAddForces()
{
    delete ui;
}

void DialogAddForces::on_pushButton_2_clicked()//添加均布力
{
    double startX = ui->lineEdit_5->text().toDouble();
    double startY = ui->lineEdit_6->text().toDouble();
    double endX = ui->lineEdit_7->text().toDouble();
    double endY = ui->lineEdit_8->text().toDouble();
    double xDirection = ui->lineEdit_9->text().toDouble();
    double yDirection = ui->lineEdit_10->text().toDouble();
    Controller::addUniformLoad(startX, startY, endX, endY, xDirection, yDirection);
}


void DialogAddForces::on_pushButton_clicked()//添加集中力
{
    Controller::addPointForce((ui->lineEdit_1->text()).toDouble(), (ui->lineEdit_2->text()).toDouble(),
        (ui->lineEdit_3->text()).toDouble(), (ui->lineEdit_4->text()).toDouble());
    QMessageBox::information(this, "操作完成", "添加成功！");
}

