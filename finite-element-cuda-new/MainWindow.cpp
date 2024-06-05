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
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QTransform>
#include <unordered_set>
#include <cfloat>
#include <QFileDialog>
#include "publicElement.h"
#include <QMessageBox>
#include <iostream>
#include <QProgressBar>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include "Controller.h"
#include <QToolBar>
#include<QToolButton>
#include <QInputDialog>
#include "ForceGraphicsItem.h"
#include <QGraphicsOpacityEffect>
#include "UniformForceGraphicsItem.h"
#include <QFile>
#include <algorithm>
#include <Map>
#include <queue>

MshInformation mshInfo;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //初始化一些状态
    mshInfo = MshInformation();
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
    setLcValue(-1);
    //设置界面标题图标
    this->setWindowTitle("有限元分析软件V1.0");
    this->setWindowIcon(QIcon(":/MainWindows/title.png"));
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
    QMenu *fileMenu = this->menuBar()->addMenu(tr("文件"));
    QAction *redoAction = fileMenu->addAction(tr("重置"));
    QAction *saveAction = fileMenu->addAction(tr("保存"));
    QAction *openAction = fileMenu->addAction(tr("打开"));
    connect(redoAction, &QAction::triggered, this, &MainWindow::clear);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveMsh);
    connect(openAction, &QAction::triggered, this, &MainWindow::openMsh);
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
    toolBarList.push_back(graphicToolBar);
    connect(addPolygonAction, &QAction::triggered, this, &MainWindow::addPolygon);
    connect(addRectAction, &QAction::triggered, this, &MainWindow::addRect);
    connect(addCircleAction, &QAction::triggered, this, &MainWindow::addCircle);
    //网格工具栏
    mshToolBar = addToolBar(tr("网格"));
    addToolBar(Qt::LeftToolBarArea, mshToolBar);
    QAction* generateMshAction = new QAction(tr("生成网格"));
    mshToolBar->addAction(generateMshAction);
    mshToolBar->setVisible(false);
    toolBarList.push_back(mshToolBar);
    connect(generateMshAction, &QAction::triggered, this, &MainWindow::generateMsh);
    //属性工具栏
    attributeToolBar = addToolBar(tr("属性"));
    addToolBar(Qt::LeftToolBarArea, attributeToolBar);
    QAction* elasticAttribAction = new QAction(tr("弹性材料属性"));
    QAction* thicknessAttribAction = new QAction(tr("材料厚度"));
    attributeToolBar->addAction(elasticAttribAction);
    attributeToolBar->addAction(thicknessAttribAction);
    attributeToolBar->setVisible(false);
    toolBarList.push_back(attributeToolBar);
    connect(elasticAttribAction, &QAction::triggered, this, &MainWindow::setElasticAttrib);
    connect(thicknessAttribAction, &QAction::triggered, this, &MainWindow::setThicknessAtrrib);
    //荷载工具栏
    loadToolBar = addToolBar(tr("荷载"));
    addToolBar(Qt::LeftToolBarArea, loadToolBar);
    QAction* addConcentratedForceAction = new QAction(tr("添加集中力"));
    QAction* addUniformForceAction = new QAction(tr("添加均布力"));
    QMenu* addForceMenu = new QMenu("添加外力", this);
    addForceMenu ->addAction(addConcentratedForceAction);
    addForceMenu->addAction(addUniformForceAction);
    QAction* forceMenuAction = new QAction("添加外力");
    forceMenuAction->setMenu(addForceMenu);
    loadToolBar->addAction(forceMenuAction);
    connect(addConcentratedForceAction, &QAction::triggered, this, &MainWindow::addConcentratedForce);
    connect(addUniformForceAction, &QAction::triggered, this, &MainWindow::addUniformForce);
    connect(ui->graphicsView, &MyGraphicsView::addConcentratedForceSignal, this, &MainWindow::showConcentratedForceInfo);
    connect(ui->graphicsView, &MyGraphicsView::addUniformForceSignal, this, &MainWindow::showUniformForceInfo);

    QAction* addEdgeAction = new QAction(tr("添加约束"));
    QAction* saveConstraintAction = new QAction(tr("保存约束"));
    QAction* openConstraintAction = new QAction(tr("装载约束"));
    loadToolBar->addAction(addEdgeAction);
    loadToolBar->addAction(saveConstraintAction);
    loadToolBar->addAction(openConstraintAction);
    loadToolBar->setVisible(false);
    toolBarList.push_back(loadToolBar);
    connect(addEdgeAction, &QAction::triggered, this, &MainWindow::addEdges);
    connect(saveConstraintAction, &QAction::triggered, this, &MainWindow::saveConstraint);
    connect(openConstraintAction, &QAction::triggered, this, &MainWindow::openConstraint);
    connect(ui->graphicsView, &MyGraphicsView::addConstraintSignal, this, &MainWindow::showInfoFromSignal);
    //计算
    calcToolBar = addToolBar(tr("计算"));
    addToolBar(Qt::LeftToolBarArea, calcToolBar);
    calcAction = new QAction(tr("计算"));
    calcToolBar->addAction(calcAction);
    calcToolBar->setVisible(false);
    toolBarList.push_back(calcToolBar);
    calcAction->setEnabled(false);
    connect(calcAction, &QAction::triggered, this, &MainWindow::calcMatrix);
    //可视化工具栏
    visualizeToolBar = addToolBar(tr("可视化"));
    addToolBar(Qt::LeftToolBarArea, visualizeToolBar);
    renderStressAction = new QAction(tr("应力云图"));
    renderUAction = new QAction(tr("水平位移云图"));
    renderVAction = new QAction(tr("竖直位移云图"));
    visualizeToolBar->addAction(renderStressAction);
    visualizeToolBar->addAction(renderUAction);
    visualizeToolBar->addAction(renderVAction);
    visualizeToolBar->setVisible(false);
    toolBarList.push_back(visualizeToolBar);
    renderStressAction->setEnabled(false);
    this->renderUAction->setEnabled(false);
    this->renderVAction->setEnabled(false);
    setToolBarStatus(GRAPH);
    connect(renderStressAction, &QAction::triggered, this, &MainWindow::renderStress);
    connect(renderUAction, &QAction::triggered, this, &MainWindow::renderU);
    connect(renderVAction, &QAction::triggered, this, &MainWindow::renderV);
    //连接槽函数
    connect(ui->graphicsView, &MyGraphicsView::createPolygonSignal, this, &MainWindow::createPolygonMsh);
    connect(ui->graphicsView, &MyGraphicsView::createRectSignal, this, &MainWindow::createRectMsh);
    connect(ui->graphicsView, &MyGraphicsView::createCircleSignal, this, &MainWindow::createCircleToMsh);
    connect(ui->graphicsView, &MyGraphicsView::resetInputAreaSignal, this, &MainWindow::resetInputArea);
    connect(ui->startInput, &QLineEdit::returnPressed, this, &MainWindow::textEntered);
    connect(this, &MainWindow::sendTextToGraphicViewSignal, ui->graphicsView, &MyGraphicsView::handleCoordinateInput);
    connect(ui->graphicsView, &MyGraphicsView::sendCalcActivate, this, &MainWindow::activateCalc);
    connect(ui->graphicsView, &MyGraphicsView::setTipsSignal, this, &MainWindow::setTips);
    connect(this, &MainWindow::updateProgressBarSignal, this, &MainWindow::updateProgressBar);
    connect(this, &MainWindow::enableRenderActionSignal, this, &MainWindow::setRenderEnable);
    connect(ui->toolBarSelectComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::onComboBoxIndexChanged);
    //绘图区   
    this->pen.setWidthF(pen.widthF() / ui->graphicsView->transform().m11());
    connect(ui->graphicsView, &MyGraphicsView::doubleClicked, this, &MainWindow::handleDoubleClick);
    MyStackedWidget* widget = ui->myStackedWidget;
    widget->setCurrentIndex(0);

    

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onComboBoxIndexChanged(int index) {
    setToolBarStatus((ToolBarStatus)index);
}
void MainWindow::setToolBarStatus(ToolBarStatus status) {
    toolBarList[curStatus]->setVisible(false);
    toolBarList[status]->setVisible(true);
    curStatus = status;
}
void MainWindow::addPolygon()
{
    while (mshInfo.lc <= 0) {
        bool ok;
        QString text = QInputDialog::getText(this, "lc未设置或者不合理", "请输入lc的值:", QLineEdit::Normal, "", &ok);
        if (!ok) {
            return;
        }
        if (ok && !text.isEmpty()) {
            this->lcValue = text.toDouble();
            setLcValue(lcValue);
        }
    }
    ui->graphicsView->setMode(QString("CREATELINE"));
    ui->myStackedWidget->setMode(QString("START"));
    ui->startInput->setFocus();
    setTips(QString("输入第一个点的坐标x y:"));
}

void MainWindow::addRect()
{
    while (mshInfo.lc <= 0) {
        bool ok;
        QString text = QInputDialog::getText(this, "lc未设置或者不合理", "请输入lc的值:", QLineEdit::Normal, "", &ok);
        if (!ok) {
            return;
        }
        if (ok && !text.isEmpty()) {
            this->lcValue = text.toDouble();
            setLcValue(lcValue);
        }
    }
    ui->graphicsView->setMode(QString("CREATERECT"));
    ui->myStackedWidget->setMode(QString("START"));
    ui->startInput->setFocus();
    setTips(QString("输入第一个点的坐标x y:"));
}

void MainWindow::addCircle()
{
    while (mshInfo.lc <= 0) {
        bool ok;
        QString text = QInputDialog::getText(this, "lc未设置或者不合理", "请输入lc的值:", QLineEdit::Normal, "", &ok);
        if (!ok) {
            return;
        }
        if (ok && !text.isEmpty()) {
            this->lcValue = text.toDouble();
            setLcValue(lcValue);
        }
    }
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
    ui->outputEdit->append("添加了一个多边形");
}

void MainWindow::createRectMsh(QPointF startPoint, QPointF endPoint)
{
    double x = startPoint.x();
    double y = startPoint.y();
    double width = endPoint.x() - x;
    double height = endPoint.y() - y;
    Controller::addRectToMsh(x, y, width, height);
    ui->outputEdit->append("添加了一个矩形");
}

void MainWindow::createCircleToMsh(double x, double y, double radius)
{
    Controller::addCircleToMsh(x, y, radius);
    ui->outputEdit->append("添加了一个圆");
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
    ui->tipsLabel->setText(msg);
}


void MainWindow::setElasticAttrib()
{
    bool ok;
    QString value1 = QInputDialog::getText(this, "输入属性值", "弹性模量:", QLineEdit::Normal, "", &ok);
    if (ok && !value1.isEmpty()) {
        this->E = value1.toDouble();
    }

    QString value2 = QInputDialog::getText(this, "输入属性值", "杨氏模量:", QLineEdit::Normal, "", &ok);
    if (ok && !value2.isEmpty()) {
        this->v = value2.toDouble();
    }
    this->setMyStatus(this->E, this->v, this->t);
    ui->outputEdit->append("设置了材料属性E=" + QString::number(this->E) + " v=" + QString::number(this->v));
}

void MainWindow::setThicknessAtrrib()
{
    bool ok;
    QString value2 = QInputDialog::getText(this, "输入属性值", "厚度:", QLineEdit::Normal, "", &ok);
    if (ok && !value2.isEmpty()) {
        this->t = value2.toDouble();
    }
    this->setMyStatus(this->E, this->v, this->t);
    ui->outputEdit->append("设置了材料厚度：t=" + QString::number(this->t));
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
        QPen tempPen = this->pen;
        tempPen.setColor(Qt::green);
        ui->graphicsView->myScene->addLine(x1, y1, x2, y2, tempPen);
    }
    ui->outputEdit->append("成功生成网格，网格数量为：" + QString::number(mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex].size())
        + "网格点数量为:" + QString::number(mshInfo.tagMap.size()));
}



void MainWindow::saveMsh()//保存网格
{
    bool success;
    QString filePath;
    Controller::saveMsh(success, filePath);
    if (success) {
        ui->outputEdit->append("已保存网格信息, 位置：" + filePath);
    }
    else {
        ui->outputEdit->append("未保存网格信息");
    }
}

void MainWindow::openMsh()//打开网格
{
    QString filePath;

    if (Controller::loadMsh(filePath)) {
        paintMsh();
        this->setMyStatus(mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex].size(), mshInfo.tagMap.size());
        ui->outputEdit->append("已加载网格信息, 位置：" + filePath);
    }
    
    else {
        ui->outputEdit->append("加载网格信息失败");
    }
}
void MainWindow::showInfoFromSignal(QString msg)
{
    ui->outputEdit->append(msg);
}
void MainWindow::addConcentratedForce()
{
    ui->graphicsView->setMode("CONCENTRATEFORCE");
    ui->myStackedWidget->setMode("START");
    ui->startInput->setFocus();
    setTips(QString("输入集中力添加的位置x y:"));
}

void MainWindow::addUniformForce()
{
    ui->graphicsView->setMode("UNIFORMFORCE");
    ui->myStackedWidget->setMode("START");
    ui->startInput->setFocus();
    setTips(QString("输入均布力的起始坐标 x y:"));
}
void MainWindow::addEdges()//添加边界条件
{
    ui->graphicsView->setMode("CONSTRAINT");
    ui->myStackedWidget->setMode("START");
    ui->startInput->setFocus();
    setTips(QString("输入添加约束的坐标 x y:"));
}

void MainWindow::saveConstraint()//保存约束
{
    bool success;
    QString filePath;
    Controller::saveConstraint(success, filePath);
    if (success) {
        ui->outputEdit->append("成功保存约束信息，位置：" + filePath);
    }
    else {
        ui->outputEdit->append("保存约束信息失败");
    }
}
void MainWindow::openConstraint()//装载约束条件的数据
{
    bool success;
    QString filePath;
    Controller::loadConstraint(success, filePath);
    this->calcAction->setEnabled(true);
    if (success) {
        for (Force &force : mshInfo.forces) {
            ui->graphicsView->handleDirectForceInput(force);
        }
        for (EdgeInfo &edgeInfo : mshInfo.edgeInfos) {
            ui->graphicsView->handleDirectConstraintInput(edgeInfo);
        }
        ui->outputEdit->append("成功加载约束信息，位置：" + filePath);
    }
    else {
        ui->outputEdit->append("加载约束信息失败");
    }
}


void MainWindow::calcMatrix()//计算k，f，uv矩阵
{
    ui->progressBar->setRange(0, 0);
    (void)QtConcurrent::run(&MainWindow::calcMatrixConcurrent, this);
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
        mshInfo.lc = -1;
    }
    else {
        lcValueLabel->setText("lc:" + QString::number(lc));
        mshInfo.lc = lc;
    }
}

void MainWindow::generateAllColorMap()
{
    //this->originItems = ui->graphicsView->myScene->items();
    //for (QGraphicsItem* graphicsItem : originItems) {
    //    this->stressItems.append(graphicsItem);
    //}
    //
    //for (QGraphicsItem* graphicsItem : originItems) {
    //    this->uItems.append(graphicsItem);
    //}
    //for (QGraphicsItem* graphicsItem : originItems) {
    //    this->vItems.append(graphicsItem);
    //}
    //生成应力云图
    double min = DBL_MAX;
    double max = -DBL_MAX;
    CalcTools::getExtreme(min, max);
    this->stressEdges[0] = max;
    this->stressEdges[1] = min;
    
    for (MechanicBehavior info : mshInfo.mechanicBehaviors) {
        double stressValue = info.equalStress; // 这里获取当前三角形网格的应力值
        double normalizedStress = (stressValue - min) / (max - min);
        normalizedStress = Controller::getColorValue(normalizedStress);
        QColor color = CalcTools::getColor(normalizedStress);
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(color);
        QPolygonF triangle;
        triangle << QPointF(info.p1.x, info.p1.y) << QPointF(info.p2.x, info.p2.y) << QPointF(info.p3.x, info.p3.y);
        QGraphicsPolygonItem* triangleItem = new QGraphicsPolygonItem(triangle);
        triangleItem->setBrush(brush);
        QPen tempPen = this->pen;
        tempPen.setColor(Qt::black);
        tempPen.setStyle(Qt::NoPen);
        triangleItem->setPen(tempPen);
        stressItems.append((QGraphicsItem*)triangleItem);
        //ui->graphicsView->scene()->addItem(triangleItem);
    }

    //生成水平位移云图
    unordered_map<Point, double> pointMap;
    priority_queue<double, vector<double>, greater<double>> valueQueue;
    for (MechanicBehavior& info : mshInfo.mechanicBehaviors) {
        double minX, minY, maxX, maxY;
        minX = std::min({ info.p1.x, info.p2.x, info.p3.x });
        maxX = std::max({ info.p1.x, info.p2.x, info.p3.x });
        minY = std::min({ info.p1.y, info.p2.y, info.p3.y });
        maxY = std::max({ info.p1.y, info.p2.y, info.p3.y });
        for (double x = minX; x <= maxX; x += 0.1) {
            for (double y = minY; y <= maxY; y += 0.1) {
                if (CalcTools::isInTriangle(x, y, info.p1.x, info.p1.y, info.p2.x, info.p2.y, info.p3.x, info.p3.y)) {
                    double u, v;

                    CalcTools::getUV(x, y, info, u, v);
                    pointMap[Point(x, y)] = u;
                    valueQueue.push(u);
                }
            }
        }
    }
    vector<double> uList;
    while (!valueQueue.empty()) {
        uList.push_back(valueQueue.top());
        valueQueue.pop();
    }
    max = uList[uList.size() - 1];
    min = uList[0];
    this->uEdges[0] = max;
    this->uEdges[1] = min;
    unordered_map<double, double> normalizedValueMap;
    vector<double> normalizedUList;
    for (double value : uList) {
        double normalizedValue = (value - min) / (max - min);
        normalizedUList.push_back(normalizedValue);
        normalizedValueMap[value] = normalizedValue;
    }
    for (auto& pair : pointMap) {
        pair.second = normalizedValueMap[pair.second];
    }
    //pointMap现在是（坐标点，归一化后的值)
    vector<double> rangeList;
    map<double, double> colorMap;
    CalcTools::evenDistribute(normalizedUList, rangeList, colorMap);
    for (auto& pair : pointMap) {
        Point point = pair.first;
        double value = pair.second;
        double targetValue = CalcTools::getMappedValue(value, rangeList, colorMap);
        QColor color = CalcTools::getColor(value);
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(color);
        QGraphicsRectItem* tempGraphicsItem = new QGraphicsRectItem(point.x - 0.05, point.y - 0.05, 0.1, 0.1);
        tempGraphicsItem->setPen(pen);
        tempGraphicsItem->setBrush(brush);
        uItems.append((QGraphicsItem*)tempGraphicsItem);
        //ui->graphicsView->myScene->addRect(point.x - 0.05, point.y - 0.05, 0.1, 0.1, pen)->setBrush(brush);
    }
    //生成竖直位移云图
    pointMap.clear();
    while (!valueQueue.empty()) {
        valueQueue.pop();
    }
    for (MechanicBehavior& info : mshInfo.mechanicBehaviors) {
        double minX, minY, maxX, maxY;
        minX = std::min({ info.p1.x, info.p2.x, info.p3.x });
        maxX = std::max({ info.p1.x, info.p2.x, info.p3.x });
        minY = std::min({ info.p1.y, info.p2.y, info.p3.y });
        maxY = std::max({ info.p1.y, info.p2.y, info.p3.y });
        for (double x = minX; x <= maxX; x += 0.1) {
            for (double y = minY; y <= maxY; y += 0.1) {
                if (CalcTools::isInTriangle(x, y, info.p1.x, info.p1.y, info.p2.x, info.p2.y, info.p3.x, info.p3.y)) {
                    double u, v;
                    CalcTools::getUV(x, y, info, u, v);
                    pointMap[Point(x, y)] = v;
                    valueQueue.push(v);
                }
            }
        }
    }
    vector<double> vList;
    while (!valueQueue.empty()) {
        vList.push_back(valueQueue.top());
        valueQueue.pop();
    }
    max = vList[vList.size() - 1];
    min = vList[0];
    this->vEdges[0] = max;
    this->vEdges[1] = min;
    normalizedValueMap.clear();
    vector<double> normalizedVList;
    for (double value : vList) {
        double normalizedValue = (value - min) / (max - min);
        normalizedVList.push_back(normalizedValue);
        normalizedValueMap[value] = normalizedValue;
    }
    for (auto& pair : pointMap) {
        pair.second = normalizedValueMap[pair.second];
    }
    //pointMap现在是（坐标点，归一化后的值)
    rangeList.clear();
    colorMap.clear();
    CalcTools::evenDistribute(normalizedVList, rangeList, colorMap);
    for (auto& pair : pointMap) {
        Point point = pair.first;
        double value = pair.second;
        double targetValue = CalcTools::getMappedValue(value, rangeList, colorMap);
        QColor color = CalcTools::getColor(value);
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(color);
        QGraphicsRectItem* tempGraphicsItem = new QGraphicsRectItem(point.x - 0.05, point.y - 0.05, 0.1, 0.1);
        tempGraphicsItem->setPen(pen);
        tempGraphicsItem->setBrush(brush);
        vItems.append((QGraphicsItem*)tempGraphicsItem);
        //ui->graphicsView->myScene->addRect(point.x - 0.05, point.y - 0.05, 0.1, 0.1, pen)->setBrush(brush);
    }

    curItemsFlag = 0;
}

void MainWindow::clearItems()
{
    if (curItemsFlag == 0) {
        return;
    }
    else if (curItemsFlag == 1) {
        for (QGraphicsItem* graphicsItem : this->stressItems) {
            ui->graphicsView->myScene->removeItem(graphicsItem);
        }
    }
    else if (curItemsFlag == 2) {
        for (QGraphicsItem* graphicsItem : this->uItems) {
            ui->graphicsView->myScene->removeItem(graphicsItem);
        }
    }
    else if (curItemsFlag == 3) {
        for (QGraphicsItem* graphicsItem : this->vItems) {
            ui->graphicsView->myScene->removeItem(graphicsItem);
        }
    }

}

void MainWindow::renderStress()//渲染
{
    clearItems();
    for (QGraphicsItem* graphicsItem : this->stressItems) {
        ui->graphicsView->myScene->addItem(graphicsItem);
    }
    ui->graphicsView->showRenderInfo(this->stressEdges[0], this->stressEdges[1]);
    curItemsFlag = 1;
}

void MainWindow::renderU() {
    clearItems();
    for (QGraphicsItem* graphicsItem : this->uItems) {
        ui->graphicsView->myScene->addItem(graphicsItem);
    }
    ui->graphicsView->showRenderInfo(this->uEdges[0], this->uEdges[1]);
    curItemsFlag = 2;
    
}

void MainWindow::renderV() {
    clearItems();
    for (QGraphicsItem* graphicsItem : this->vItems) {
        ui->graphicsView->myScene->addItem(graphicsItem);
    }
    ui->graphicsView->showRenderInfo(this->vEdges[0], this->vEdges[1]);
    curItemsFlag = 3;
}

void MainWindow::clear()//清空所有信息
{
    //originItems.clear();
    //stressItems.clear();
    //uItems.clear();
    //vItems.clear();
    QList<QGraphicsItem*> tempItemList = ui->graphicsView->myScene->items();
    for (QGraphicsItem* graphicsItem : tempItemList) {
        ui->graphicsView->myScene->removeItem(graphicsItem);
    }
    while (!originItems.isEmpty()) {
        QGraphicsItem* graphicsItem = originItems.first();
        delete graphicsItem;
        originItems.removeFirst();
    }
    while (!stressItems.isEmpty()) {
        QGraphicsItem* graphicsItem = stressItems.first();
        delete graphicsItem;
        stressItems.removeFirst();
    }
    while (!uItems.isEmpty()) {
        QGraphicsItem* graphicsItem = uItems.first();
        delete graphicsItem;
        uItems.removeFirst();
    }
    while (!vItems.isEmpty()) {
        QGraphicsItem* graphicsItem = vItems.first();
        delete graphicsItem;
        vItems.removeFirst();
    }
    gmsh::finalize();
    this->calcAction->setEnabled(false);
    this->renderStressAction->setEnabled(false);
    this->renderUAction->setEnabled(false);
    this->renderVAction->setEnabled(false);
    ui->progressBar->setValue(0);
    mshInfo.clearAll();
    ui->graphicsView->gradientBox->hide();
    

    gmsh::initialize();
    this->setMyStatus(18000000000, 0.25, 1);
    this->setMyStatus(0, 0);
    setLcValue(-1);
}

void MainWindow::handleDoubleClick(QPointF  point)//点击网格中的点，然后显示出相关信息
{
        double x = point.x();
        double y = point.y();
        MechanicBehavior mechanicBehavior;
        double u;
        double v;
        if (CalcTools::getPointInfos(x, y, mechanicBehavior, u , v)) {
            
            double min = DBL_MAX;
            double max = -DBL_MAX;
            CalcTools::getExtreme(min, max);
            double stressValue = mechanicBehavior.equalStress; // 这里获取当前三角形网格的应力值
            double normalizedStress = (stressValue - min) / (max - min);
            normalizedStress = Controller::getColorValue(normalizedStress);
            
            
            QString htmlContent = htmlTemplate.arg(x).arg(y).arg(u).arg(v).arg(mechanicBehavior.strain(0, 0))
                .arg(mechanicBehavior.strain(1, 0)).arg(mechanicBehavior.strain(2, 0)).arg(mechanicBehavior.equalStrain)
                .arg(mechanicBehavior.stress(0, 0)).arg(mechanicBehavior.stress(1, 0)).
                arg(mechanicBehavior.stress(2, 0)).arg(mechanicBehavior.equalStress);
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
    this->renderStressAction->setEnabled(enable);
    this->renderUAction->setEnabled(enable);
    this->renderVAction->setEnabled(enable);
    this->generateAllColorMap();
}

void MainWindow::updateProgressBar(int value)
{
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(value);
    ui->outputEdit->append("计算完成");
    //计算应力应变
    CalcTools::calcStressStrain();
    //计算完成后就开始实现颜色映射算法
    Controller::generateColorMap();
}

void MainWindow::showConcentratedForceInfo(double x, double y, double xForce, double yForce)
{
    ui->outputEdit->append("添加了集中力：x:" + QString::number(x) + ", y:" + QString::number(y)+ ", 水平作用力：" + 
        QString::number(xForce) + ",竖直作用力：" + QString::number(yForce));
}

void MainWindow::showUniformForceInfo(double startX, double startY, double endX, double endY, double xForce, double yForce, vector<Force> forces) {
    ui->outputEdit->append("添加了均布力：起始位置x:" + QString::number(startX) + ", 起始坐标y:" + QString::number(startY) + 
        "终点坐标x:" + QString::number(endX) + "终点坐标y:" + QString::number(endY) + ", 水平作用力：" +
        QString::number(xForce) + ",竖直作用力：" + QString::number(yForce));
    for (Force force : forces) {
        ui->graphicsView->handleDirectForceInput(force);
    }
    
}