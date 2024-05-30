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
#include <QToolBar>
#include<QToolButton>

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
    // 添加状态栏信息
    permanentLabel = new QLabel("", this);
    paramsLabel = new QLabel("", this);
    lcValueLabel = new QLabel("", this);
    this->statusBar()->addPermanentWidget(permanentLabel);
    this->statusBar()->addPermanentWidget(paramsLabel);
    this->statusBar()->addPermanentWidget(this->lcValueLabel);
    //初始化状态栏信息
    this->setMyStatus(18000000000, 0.25, 1);
    this->setMyStatus(0, 0);
    setLcValue(0.5);
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
    //设置布局
    ui->splitter->setStretchFactor(7, 3);
    QList<int> sizes;
    sizes << 7 << 3;
    ui->splitter->setSizes(sizes);
    //添加菜单

    QMenu *fileMenu = this->menuBar()->addMenu(tr("模型"));
    QAction *redoAction = fileMenu->addAction(tr("重置"));
    QAction *saveAction = fileMenu->addAction(tr("保存"));
    QAction *openAction = fileMenu->addAction(tr("打开"));


    QAction *generateMshAction = this->menuBar()->addAction(tr("生成网格"));


    calcAction = this->menuBar()->addAction(tr("计算"));
    renderAction = this->menuBar()->addAction(tr("渲染"));
    renderAction->setEnabled(false);
    calcAction->setEnabled(false);


    //工具栏
    //图形工具栏
    graphicToolBar = addToolBar(tr("图形创建"));
    addToolBar(Qt::LeftToolBarArea, graphicToolBar);
    QAction* addPolygonAction = new QAction(this);
    addPolygonAction->setIcon(QIcon(":/MainWindows/connect-line.png"));
    QAction* addRectAction = new QAction(this);
    addRectAction->setIcon(QIcon(":/MainWindows/rect.png"));
    QAction* addCircleAction = new QAction(this);
    addCircleAction->setIcon(QIcon(":/MainWindows/circle.png"));
    graphicToolBar->addAction(addPolygonAction);
    graphicToolBar->addAction(addRectAction);
    graphicToolBar->addAction(addCircleAction);
    graphicToolBar->setVisible(false);
    //属性工具栏
    attributeToolBar = addToolBar(tr("属性"));
    addToolBar(Qt::LeftToolBarArea, attributeToolBar);

    //荷载工具栏
    loadToolBar = addToolBar(tr("荷载"));
    addToolBar(Qt::LeftToolBarArea, loadToolBar);
    QAction* addForceAction = new QAction(tr("添加外力"));
    QAction* addEdgeAction = new QAction(tr("添加边界条件"));
    QAction* saveConstraintAction = new QAction(tr("保存约束"));
    QAction* openConstraintAction = new QAction(tr("装载约束"));
    loadToolBar->addAction(addForceAction);
    loadToolBar->addAction(addEdgeAction);
    loadToolBar->addAction(saveConstraintAction);
    loadToolBar->addAction(openConstraintAction);
    //可视化工具栏
    visualizeToolBar = addToolBar(tr("可视化"));
    addToolBar(Qt::LeftToolBarArea, visualizeToolBar);

    connect(addPolygonAction, &QAction::triggered, this, &MainWindow::addPolygon);
    connect(addRectAction, &QAction::triggered, this, &MainWindow::addRect);
    connect(addCircleAction, &QAction::triggered, this, &MainWindow::addCircle);
    connect(ui->graphicsView, &MyGraphicsView::createPolygonSignal, this, &MainWindow::createPolygonMsh);
    connect(ui->graphicsView, &MyGraphicsView::createRectSignal, this, &MainWindow::createRectMsh);
    connect(ui->graphicsView, &MyGraphicsView::createCircleSignal, this, &MainWindow::createCircleToMsh);
    connect(ui->graphicsView, &MyGraphicsView::resetInputAreaSignal, this, &MainWindow::resetInputArea);
    connect(ui->startInput, &QLineEdit::returnPressed, this, &MainWindow::textEntered);
    connect(this, &MainWindow::sendTextToGraphicViewSignal, ui->graphicsView, &MyGraphicsView::handleCoordinateInput);
    connect(ui->graphicsView, &MyGraphicsView::setTipsSignal, this, &MainWindow::setTips);
    //绘图区   
    this->pen.setWidthF(pen.widthF() / ui->graphicsView->transform().m11());
    connect(ui->graphicsView, &MyGraphicsView::doubleClicked, this, &MainWindow::handleDoubleClick);

    //链接槽函数
    connect(redoAction, &QAction::triggered, this, &MainWindow::clear);
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



void MainWindow::addPolygon()
{
    if (lcValue == -1) {
        QMessageBox::warning(this, "警告", "lc不能为空，请输入有效的内容。");
        return; // 退出当前函数
    }
    mshInfo.lc = this->lcValue;
    ui->graphicsView->setMode(QString("CREATELINE"));
    ui->myStackedWidget->setMode(QString("START"));
    ui->startInput->setFocus();
    setTips(QString("输入第一个点的坐标x y:"));
}

void MainWindow::addRect()
{
    if (this->lcValue == -1) {
        QMessageBox::warning(this, "警告", "lc不能为空，请输入有效的内容。");
        return; // 退出当前函数
    }
    mshInfo.lc = this->lcValue;
    ui->graphicsView->setMode(QString("CREATERECT"));
    ui->myStackedWidget->setMode(QString("START"));
    ui->startInput->setFocus();
    setTips(QString("输入第一个点的坐标x y:"));
}

void MainWindow::addCircle()
{
    if (this->lcValue == -1) {
        QMessageBox::warning(this, "警告", "lc不能为空，请输入有效的内容。");
        return; // 退出当前函数
    }
    mshInfo.lc = lcValue;
    ui->graphicsView->setMode(QString("CREATECIRCLE"));
    ui->myStackedWidget->setMode(QString("START"));
    ui->startInput->setFocus();
    setTips(QString("输入圆心的坐标x y:"));
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

void MainWindow::createRectMsh(QPointF startPoint, QPointF endPoint)
{
    double x = startPoint.x();
    double y = startPoint.y();
    double width = endPoint.x() - x;
    double height = endPoint.y() - y;
    Controller::addRectToMsh(x, y, width, height);
}

void MainWindow::createCircleToMsh(double x, double y, double radius)
{
    Controller::addCircleToMsh(x, y, radius);
}

void MainWindow::textEntered()//输入xy值
{
    QString text = ui->startInput->text();
    ui->startInput->clear();
    emit sendTextToGraphicViewSignal(text);
}
void MainWindow::resetInputArea()
{
    ui->myStackedWidget->setMode(QString("INIT"));
}

void MainWindow::setTips(const QString &msg)
{
    QString test = QString("这是段测试代码");
    ui->tipsLabel->setText(msg);
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
    this->setMyStatus(mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex].size(), mshInfo.tagMap.size());
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
        this->setMyStatus(mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex].size(), mshInfo.tagMap.size());
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

    Controller::generateMatrixes(this->E, this->v, this->t);

    emit updateProgressBarSignal(100);
    emit enableRenderActionSignal(true);
}

void MainWindow::setMyStatus(double E, double v, double t)
{
    paramsLabel->setText("E:" + QString::number(E, 'e', 2) + " V:" + QString::number(v) + " t:" + QString::number(t));
    this->E = E;
    this->v = v;
    this->t = t;
}

void MainWindow::setMyStatus(double meshNums, double pointNums)
{
    this->permanentLabel->setText("网格数量为：" + QString::number(meshNums) + "网格点数量为:" + QString::number(pointNums));
}
void MainWindow::setLcValue(double lc)
{
    if (lc < 0) {
        lcValueLabel->setText("lc未设置");
        lcValue = -1;
    }
    else {
        lcValueLabel->setText("lc:" + QString::number(lc));
        lcValue = lc;
    }
}

void MainWindow::render()//渲染
{
    CalcTools::calcStressStrain();
    double min = DBL_MAX;
    double max = -DBL_MAX;

    CalcTools::getExtreme(min , max);
    ui->graphicsView->showRenderInfo(max, min);
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
}


void MainWindow::clear()//清空所有信息
{
    ui->graphicsView->scene()->clear();
    gmsh::finalize();
    this->calcAction->setEnabled(false);
    this->renderAction->setEnabled(false);
    ui->progressBar->setValue(0);
    mshInfo.clearAll();
    this->setMyStatus(0, 0);
}

void MainWindow::handleDoubleClick(QPointF  point)//点击网格中的点，然后显示出相关信息
{
        double x = point.x();
        double y = point.y();
        MechanicBehavior mechanicBehavior;
        double u;
        double v;
        if (CalcTools::getPointInfos(x, y, mechanicBehavior, u , v)) {
            QString htmlContent = htmlTemplate.arg(x).arg(y).arg(mechanicBehavior.stress(0, 0)).arg(mechanicBehavior.stress(1, 0)).
                arg(mechanicBehavior.stress(2, 0)).arg(mechanicBehavior.strain(0, 0)).arg(mechanicBehavior.strain(1, 0)).arg(mechanicBehavior.strain(2, 0)).
                arg(u).arg(v);
            QTextCursor cursor = ui->outputEdit->textCursor();
            cursor.movePosition(QTextCursor::End);
            ui->outputEdit->append(htmlContent);
            cursor.movePosition(QTextCursor::Down);
            ui->outputEdit->setTextCursor(cursor);
            ui->outputEdit->ensureCursorVisible();
        } else {
            QString htmlContent = htmlTemplate.arg(x).arg(y).arg(0).arg(0).
                arg(0).arg(0).arg(0).arg(0).
                arg(0).arg(0);
            QTextCursor cursor = ui->outputEdit->textCursor();
            cursor.movePosition(QTextCursor::End);
            ui->outputEdit->append(htmlContent);
            cursor.movePosition(QTextCursor::Down);
            ui->outputEdit->setTextCursor(cursor);
            ui->outputEdit->ensureCursorVisible();
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

