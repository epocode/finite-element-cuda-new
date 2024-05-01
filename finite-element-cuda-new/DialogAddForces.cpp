#include "DialogAddForces.h"
#include "ui_DialogAddForces.h"
#include "publicElement.h"
#include "MshInformation.h"
#include <queue>
#include "PublicTools.h"
#include <QMessageBox>

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


struct ComparePair {
    bool operator()(const std::pair<double, double>& a, const std::pair<double, double>& b) const {
        return a.first > b.first; // 按照pair的first值从小到大排序
    }
};

void DialogAddForces::on_pushButton_2_clicked()//添加均布力
{
    double startX = ui->lineEdit_5->text().toDouble();
    double startY = ui->lineEdit_6->text().toDouble();
    double endX = ui->lineEdit_7->text().toDouble();
    double endY = ui->lineEdit_8->text().toDouble();
    std::priority_queue<pair<double, double>, vector<pair<double,double>>, ComparePair> pq;
    pq.push(pair<double, double>(startX, startY));
    pq.push(pair<double, double>(endX, endY));
    double xDirection = ui->lineEdit_9->text().toDouble();
    double yDirection = ui->lineEdit_10->text().toDouble();
    for (int i = 0; i < mshInfo.xList.size(); i++) {
        double x = mshInfo.xList[i];
        double y = mshInfo.yList[i];
        double instance = pointToSegmentDistance(startX, startY, endX, endY, x, y);
        if (instance < 1e-5) {
            pq.push(pair<double, double>(x, y));
        }
    }
    while (pq.size() > 1) {
        pair<double, double> pre = pq.top();
        pq.pop();
        pair<double, double> next = pq.top();
        Force force;
        force.x = (pre.first + next.first) / 2;
        force.y = (pre.second + next.second) / 2;
        double len = sqrt(pow(pre.first - next.first, 2) + pow(pre.second - next.second, 2));
        force.xForce = len * xDirection;
        force.yForce = len * yDirection;
        mshInfo.forces.push_back(force);
    }
    QMessageBox::information(this, "操作完成", "添加成功！");
}


void DialogAddForces::on_pushButton_clicked()//添加集中力
{
    Force force;
    force.x = (ui->lineEdit_1->text()).toDouble();
    force.y = (ui->lineEdit_2->text()).toDouble();
    force.xForce = (ui->lineEdit_3->text()).toDouble();
    force.yForce = (ui->lineEdit_4->text()).toDouble();
    mshInfo.forces.push_back(force);
    QMessageBox::information(this, "操作完成", "添加成功！");
}

