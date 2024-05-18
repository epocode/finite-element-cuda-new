#include "DialogEdgeAdd.h"
#include "ui_DialogEdgeAdd.h"
#include "MshInformation.h"
#include "CalcTools.h"
#include "Controller.h"

extern MshInformation mshInfo;

DialogEdgeAdd::DialogEdgeAdd(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogEdgeAdd)
{
    ui->setupUi(this);
}

DialogEdgeAdd::~DialogEdgeAdd()
{
    delete ui;
}

void DialogEdgeAdd::on_buttonBox_accepted()
{
    Controller::addEdges(ui->lineEdit->text().toDouble(), ui->lineEdit_2->text().toDouble(),
        ui->x_direction_fixed->isChecked(), ui->y_direction_fixed->isChecked());
    emit sendCalcActivate();
}
