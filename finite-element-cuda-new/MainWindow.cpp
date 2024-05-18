#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMenu>
#include <QLine>
#include "publicElement.h"
#include <gmsh.h>
#include "MshInformation.h"
#include "CalcTools.h"
#include <fstream>
#include <QCoreApplication>
#include <QtConcurrent/QtConcurrent>
#include <QThread>
#include "DialogEdgeAdd.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QTransform>
#include <unordered_set>
#include <cfloat>
#include <QFileDialog>
#include "DialogAddGraphics.h"
#include "publicElement.h"
#include <QMessageBox>
#include "DialogAddForces.h"
#include <iostream>
#include <QProgressBar>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include "Controller.h"

extern MshInformation mshInfo;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //初始化一些状态
    gmsh::initialize();
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);
    this->paintState = true;    //初始化绘图状态
    // 添加状态栏信息
    permanentLabel = new QLabel("网格数量为：", this);
    this->statusBar()->addPermanentWidget(permanentLabel);

    //设置界面标题图标
    this->setWindowTitle("有限元分析软件V1.0");
    this->setWindowIcon(QIcon(":/MainWindows/title.png"));
    //设置界面的qss
    QFile file(":/MainWindows/ManjaroMix.qss");
    file.open(QFile::ReadOnly);
    QString qss = file.readAll();
    this->setStyleSheet(qss);
    file.close();
    //设置窗口位置大小
    int width = 1000;
    int height = 600;
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width) / 2;
    int y = (screenGeometry.height() - height) / 2;
    this->setGeometry(x, y, width, height);
    //添加菜单

    QMenu *fileMenu = this->menuBar()->addMenu(tr("模型"));
    QAction *redoAction = fileMenu->addAction(tr("重置"));
    QAction *saveAction = fileMenu->addAction(tr("保存"));
    QAction *openAction = fileMenu->addAction(tr("打开"));

    QMenu *graphicsMenu = this->menuBar()->addMenu(tr("图形"));
    QAction *addFixedGraphicsAction = graphicsMenu->addAction(tr("添加固定图形"));
    QAction* addPolygonAction = graphicsMenu->addAction(tr("添加多边形"));


    QAction *generateMshAction = this->menuBar()->addAction(tr("生成网格"));

    QMenu *calcMenu = this->menuBar()->addMenu(tr("约束"));
    QAction *addForceAction = calcMenu->addAction(tr("添加外力"));
    QAction *addEdgeAction = calcMenu->addAction(tr("添加边界条件"));
    QAction *saveConstraintAction = calcMenu->addAction(tr("保存约束"));
    QAction *openConstraintAction = calcMenu->addAction(tr("装载约束"));
    calcAction = this->menuBar()->addAction(tr("计算"));
    renderAction = this->menuBar()->addAction(tr("渲染"));
    renderAction->setEnabled(false);
    calcAction->setEnabled(false);

    //绘图区   
    this->pen.setWidthF(pen.widthF() / ui->graphicsView->transform().m11());
    //处理绘图区的鼠标操作
    connect(ui->graphicsView, &MyGraphicsView::doubleClicked, this, &MainWindow::handleDoubleClick);
    //初始化输入框的内容
    ui->lineEditE->setText(QString::number(18000000000, 'e', 2));
    ui->lineEditV->setText("0.25");
    ui->lineEditT->setText("1");

  

    //链接槽函数
    connect(redoAction, &QAction::triggered, this, &MainWindow::clear);
    connect(addFixedGraphicsAction, &QAction::triggered, this, &MainWindow::addGraphics);
    connect(addPolygonAction, &QAction::triggered, this, &MainWindow::addPolygon);
    connect(ui->startInput, &QLineEdit::returnPressed, this, &MainWindow::textEntered);
    connect(this, &MainWindow::sendTextToGraphicViewSignal, ui->graphicsView, &MyGraphicsView::handleCoordinateInput);
    connect(ui->graphicsView, &MyGraphicsView::createPolygonSignal, this, &MainWindow::createPolygonMsh);


    connect(generateMshAction, &QAction::triggered, this, &MainWindow::generateMsh);
    connect(addForceAction, &QAction::triggered, this, &MainWindow::addForces);
    connect(addEdgeAction, &QAction::triggered, this,&MainWindow::addEdges);
    connect(calcAction, &QAction::triggered, this, &MainWindow::calcMatrix);
    connect(renderAction, &QAction::triggered, this, &MainWindow::render);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveMsh);
    connect(openAction, &QAction::triggered, this, &MainWindow::openMsh);
    connect(saveConstraintAction, &QAction::triggered, this, &MainWindow::saveConstraint);
    connect(openConstraintAction, &QAction::triggered, this,&MainWindow::openConstraint);
    connect(this, &MainWindow::updateProgressBarSignal, this, &MainWindow::updateProgressBar);
    connect(this, &MainWindow::enableRenderActionSignal, this, &MainWindow::setRenderEnable);
    
    MyStackedWidget* widget = ui->myStackedWidget;
    widget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//创建图形
void MainWindow::addGraphics()
{
    if (ui->lcValue->text().isEmpty()) {
        QMessageBox::warning(this, "警告", "lc不能为空，请输入有效的内容。");
        return; // 退出当前函数
    }
    double lc = ui->lcValue->text().toDouble();
    mshInfo.lc = lc;
    DialogAddGraphics* dialog = new DialogAddGraphics(nullptr, lc);
    dialog->setLayout(this->layout());
    dialog->setWindowTitle("添加图形");
    QPoint cursorPos = QCursor::pos();
    dialog->move(cursorPos);
    dialog->show();
    connect(dialog, &DialogAddGraphics::sendRectSignal, this, &MainWindow::paintRect);
    connect(dialog, &DialogAddGraphics::sendCircleSignal, this, &MainWindow::paintCircle);
}
void MainWindow::addPolygon()
{
    if (ui->lcValue->text().isEmpty()) {
        QMessageBox::warning(this, "警告", "lc不能为空，请输入有效的内容。");
        return; // 退出当前函数
    }
    double lc = ui->lcValue->text().toDouble();
    mshInfo.lc = lc;
    ui->graphicsView->setMode(QString("CREATELINE"));
    ui->myStackedWidget->setMode(QString("START"));
    ui->startInput->setFocus();
}

void MainWindow::textEntered()//输入xy值
{
    QString text = ui->startInput->text();
    ui->startInput->clear();
    emit sendTextToGraphicViewSignal(text);
}

void MainWindow::createPolygonMsh(QVector<QPointF> points)
{
    vector<Coordinate> newPoints;
    for (QPointF point : points) {
        double x = point.x();
        double y = point.y();
        Coordinate coordinate;
        coordinate.x = x;
        coordinate.y = y;
        newPoints.push_back(coordinate);
    }
    ui->myStackedWidget->setMode(QString("INIT"));
    Controller::addPolygonToMsh(newPoints);
}

//绘图区绘制四边形
void MainWindow::paintRect(Rect rect)
{
    QGraphicsRectItem* rectItem = ui->graphicsView->scene()->addRect(rect.x, rect.y, rect.width, rect.height);
    rectItem->setPen(this->pen);
}
//绘制圆形
void MainWindow::paintCircle(Circle circle)
{
    QGraphicsEllipseItem* circleItem = ui->graphicsView->scene()->addEllipse(circle.x - circle.radius, circle.y - circle.radius,
        circle.radius * 2, circle.radius * 2);
    circleItem->setPen(this->pen);
}

void MainWindow::generateMsh()//生成网格(从输入的图形中生成)
{
    //如果gmsh已经有了图形或者没有curveLoop，那么将什么也不干；
    if (mshInfo.nodeTags.size() != 0 || mshInfo.curveLoopList.size() == 0) {
        return;
    }
    Controller::generateMsh();

    //初始化网格点和三角形的信息
    mshInfo.initPointAndTriangleInfo();
    this->paintMsh();
    this->permanentLabel->setText("网格数量为：" + QString::number(mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex].size()) + "网格点数量为:" + QString::number(mshInfo.tagMap.size()));

}



void MainWindow::paintMsh()//绘制网格(被其他槽函数调用,这是从msh自带的网格信息中的三角形生成的)
{
    unordered_set<pair<int, int>, pair_hash> list;
    for (int i = 0; i < mshInfo.elementTags[mshInfo.triangleIndex].size(); i++) {
        int index1 = mshInfo.tagMap[mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex][i * 3 + 0]];
        int index2 = mshInfo.tagMap[mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex][i * 3 + 1]];
        int index3 = mshInfo.tagMap[mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex][i * 3 + 2]];
        if (list.find(make_pair(index1, index2)) == list.end() && list.find(make_pair(index2, index1)) == list.end()) {
            list.insert(make_pair(index1, index2));
        }
        if (list.find(make_pair(index1, index3)) == list.end() && list.find(make_pair(index3, index1)) == list.end()) {
            list.insert(make_pair(index1, index3));
        }
        if (list.find(make_pair(index2, index3)) == list.end() && list.find(make_pair(index3, index2)) == list.end()) {
            list.insert(make_pair(index2, index3));
        }
    }
    for (auto p : list) {
        //qDebug() << p.first << "\t" << p.second;
        double x1 = mshInfo.xList[p.first];
        double y1 = mshInfo.yList[p.first];
        double x2 = mshInfo.xList[p.second];
        double y2 = mshInfo.yList[p.second];
        this->pen.setColor(Qt::green);
        ui->graphicsView->scene()->addLine(x1, y1, x2, y2, this->pen);
    }
}


void MainWindow::saveMsh()//保存网格
{
    Controller::saveMsh();
}

void MainWindow::openMsh()//打开网格
{
    if (Controller::loadMsh()) {
        paintMsh();
        this->permanentLabel->setText("网格数量为：" + QString::number(mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex].size()) + "网格点数量为:" + QString::number(mshInfo.tagMap.size()));
    }
}
void MainWindow::addForces()//添加外力
{
    DialogAddForces* dialog = new DialogAddForces;
    dialog->setWindowTitle("添加外力");
    QPoint cursorPos = QCursor::pos();
    dialog->move(cursorPos);
    dialog->show();
}

void MainWindow::addEdges()//添加边界条件
{
    DialogEdgeAdd* dialog = new DialogEdgeAdd;
    dialog->setWindowTitle("添加边界条件");
    QPoint cursorPos = QCursor::pos();
    dialog->move(cursorPos);
    dialog->show();
    connect(dialog, &DialogEdgeAdd::sendCalcActivate, this, &MainWindow::activateCalc);

}

void MainWindow::saveConstraint()//保存约束
{
    Controller::saveConstraint();
}
void MainWindow::openConstraint()//装载约束条件的数据
{
    Controller::loadConstraint();
    this->calcAction->setEnabled(true);
}


void MainWindow::calcMatrix()//计算k，f，uv矩阵
{
    ui->progressBar->setRange(0, 0);
    QtConcurrent::run(&MainWindow::calcMatrixConcurrent, this);
}
void MainWindow::calcMatrixConcurrent()//多线程计算矩阵
{
    QString E = ui->lineEditE->text();
    QString v = ui->lineEditV->text();
    QString t = ui->lineEditT->text();
    if (E.isEmpty() || v.isEmpty() || t.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请正确输入Evt的值"));
        return;
    }
    Controller::generateMatrixes(E.toDouble(), v.toDouble(), t.toDouble());

    emit updateProgressBarSignal(100);
    emit enableRenderActionSignal(true);
}

void MainWindow::render()//渲染
{
    CalcTools::calcStressStrain();
    double min = DBL_MAX;
    double max = -DBL_MAX;

    CalcTools::getExtreme(min , max);
    qDebug() << qSetRealNumberPrecision(2) << scientific << "min: " << min << ", max: " << max;
    ui->lineEditMin->setText(QString::number(min, 'e', 2));
    ui->lineEditMax->setText(QString::number(max, 'e', 2));
    for (MechanicBehavior info: mshInfo.mechanicBehaviors) {
        double stressValue = info.equalStress; // 这里获取当前三角形网格的应力值
        double normalizedStress = (stressValue - min) / (max - min);
        double factor = 0.5; // 可以调整这个因子来改变颜色的深浅
        int red = 0;
        int green = 0;
        int blue = 0;
        if (normalizedStress <= 0.5) {
            green = static_cast<int>(255 * (1 - normalizedStress * 2));
            red = static_cast<int>(255 * (1 - factor * (1 - normalizedStress * 2)));
            blue = 0;
        }
        else {
            green = static_cast<int>(255 * (1 - factor * ((normalizedStress - 0.5) * 2)));
            red = 255;
            blue = 0;
        }
        QColor color(red, green, blue);
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(color);
        QPolygonF triangle;
        triangle << QPointF(info.p1.x, info.p1.y) << QPointF(info.p2.x, info.p2.y) << QPointF(info.p3.x, info.p3.y);
        QGraphicsPolygonItem *triangleItem = new QGraphicsPolygonItem(triangle);
        triangleItem->setBrush(brush);
        this->pen.setColor(Qt::black);
        triangleItem->setPen(this->pen);
        ui->graphicsView->scene()->addItem(triangleItem);
    }
    QLinearGradient gradient(0, 0, ui->colorBar->width(), 0);
    gradient.setColorAt(0, Qt::green); // 应力最小值对应红色
    gradient.setColorAt(0.5, Qt::blue); // 应力中间值对应绿色
    gradient.setColorAt(1, Qt::red);
    QPalette palette = ui->colorBar->palette();

    // 使用渐变作为 QPalette 的笔刷
    palette.setBrush(QPalette::Window, QBrush(gradient));

    // 将 QPalette 应用到 QLabel 上
    ui->colorBar->setAutoFillBackground(true); // 确保背景被填充
    ui->colorBar->setPalette(palette);

    // 更新 QLabel 以显示新的背景色
    ui->colorBar->update();
}



void MainWindow::clear()//清空所有信息
{
    ui->graphicsView->scene()->clear();
    gmsh::finalize();
    this->calcAction->setEnabled(false);
    this->renderAction->setEnabled(false);
    ui->progressBar->setValue(0);
    mshInfo.clearAll();
    this->permanentLabel->setText("网格数量为：" + QString::number(0) + "网格点数量为:" + QString::number(0));
}

void MainWindow::handleDoubleClick(QPointF  point)//点击网格中的点，然后显示出相关信息
{
        double x = point.x();
        double y = point.y();
        ui->lineEditPointX->setText(QString::number(x));
        ui->lineEditPointY->setText(QString::number(y));
        MechanicBehavior mechanicBehavior;
        double u;
        double v;
        if (CalcTools::getPointInfos(x, y, mechanicBehavior, u , v)) {
            ui->sigmaX->setText(QString::number(mechanicBehavior.stress(0, 0), 'e', 2));
            ui->sigmaY->setText(QString::number(mechanicBehavior.stress(1, 0), 'e', 2));
            ui->tau->setText(QString::number(mechanicBehavior.stress(2, 0), 'e', 2));
            ui->epsilonX->setText(QString::number(mechanicBehavior.strain(0, 0), 'e', 2));
            ui->epsilonY->setText(QString::number(mechanicBehavior.strain(1, 0), 'e', 2));
            ui->gamma->setText(QString::number(mechanicBehavior.strain(2, 0), 'e', 2));
            ui->mu->setText(QString::number(u, 'e', 2));
            ui->nu->setText(QString::number(v, 'e', 2));
        } else {
            ui->sigmaX->setText(QString::number( 0));
            ui->sigmaY->setText(QString::number(0));
            ui->tau->setText(QString::number(0));
            ui->epsilonX->setText(QString::number(0));
            ui->epsilonY->setText(QString::number(0));
            ui->gamma->setText(QString::number(0));
            ui->mu->setText(QString::number(0));
            ui->nu->setText(QString::number(0));
        }


}

void MainWindow::activateCalc()
{
    this->calcAction->setEnabled(true);
}

void MainWindow::setRenderEnable(bool enable)
{
    this->renderAction->setEnabled(enable);
}

void MainWindow::updateProgressBar(int value)
{
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(value);
}

