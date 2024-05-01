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
#include "QGraphicsViewEventFilter.h"
#include <QFile>
// #include <QtConcurrent>

extern MshInformation mshInfo;
extern "C" void solveMatrix(Eigen::MatrixXd & kMatrix, Eigen::MatrixXd & fMatrix, Eigen::MatrixXd & uvMatrix);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    //布局设置
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);

    QMenu *fileMenu = this->menuBar()->addMenu(tr("模型"));
    QAction *redoAction = fileMenu->addAction(tr("重置"));
    QAction *saveAction = fileMenu->addAction(tr("保存"));
    QAction *openAction = fileMenu->addAction(tr("打开"));

    QAction *addGraphicsAction = this->menuBar()->addAction(tr("添加图形"));
    QAction *generateMshAction = this->menuBar()->addAction(tr("生成网格"));

    QMenu *calcMenu = this->menuBar()->addMenu(tr("约束"));
    QAction *addForceAction = calcMenu->addAction(tr("添加外力"));
    QAction *addEdgeAction = calcMenu->addAction(tr("添加边界条件"));
    QAction *saveConstraintAction = calcMenu->addAction(tr("保存约束"));
    QAction *openConstraintAction = calcMenu->addAction(tr("装载约束"));

    calcAction = this->menuBar()->addAction(tr("计算"));


    renderAction = this->menuBar()->addAction(tr("渲染"));

    // 创建一个QLabel对象
    permanentLabel = new QLabel("网格数量为：", this);

    // 将QLabel添加到状态栏的永久区域
    this->statusBar()->addPermanentWidget(permanentLabel);


    renderAction->setEnabled(false);
    calcAction->setEnabled(false);
    //界面类初始化
    this->setWindowTitle("有限元分析软件V1.0");
    this->setWindowIcon(QIcon(":/MainWindows/title.png"));
    QGraphicsScene * graphicsScene = new QGraphicsScene;
    ui->graphicsView->scale(1, -1);
    ui->graphicsView->setScene(graphicsScene);
    ui->graphicsView->scale(40, 40);
    this->pen.setWidthF(pen.widthF() / ui->graphicsView->transform().m11());
    (ui->graphicsView->scene()->addLine(QLineF(0, 0, 0, 100), QPen(Qt::red)))->setPen(this->pen);
    (ui->graphicsView->scene()->addLine(QLineF(0, 0, 100, 0), QPen(Qt::red)))->setPen(this->pen);
    ui->graphicsView->show();
    qDebug() << "created the Qgraphics";
    ui->lineEditE->setText(QString::number(18000000000, 'e', 2));
    ui->lineEditV->setText("0.25");
    ui->lineEditT->setText("1");

    GraphicsViewEventFilter *filter = new GraphicsViewEventFilter(this);
    ui->graphicsView->installEventFilter(filter);
    connect(filter, &GraphicsViewEventFilter::clicked, this, &MainWindow::handleClick);

    //链接槽函数
    connect(redoAction, &QAction::triggered, this, &MainWindow::clear);
    connect(addGraphicsAction, &QAction::triggered, this, &MainWindow::addGraphics);
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
    
    //初始化绘图状态
    this->paintState = true;
}

MainWindow::~MainWindow()
{
    delete ui;
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
    mshInfo.E = E.toDouble();
    mshInfo.v = v.toDouble();
    mshInfo.t = t.toDouble();
    calcKMatrix();
    calcFMatrix();
    handleEdge();
    //mshInfo.X = mshInfo.kMatrix.colPivHouseholderQr().solve(mshInfo.fMatrix);
    solveMatrix(mshInfo.kMatrix, mshInfo.fMatrix, mshInfo.X);
    std::ofstream file("Xmatrix.txt");
    if (file.is_open()) {
        for (int i = 0; i < mshInfo.X.rows(); ++i) {
            for (int j = 0; j < mshInfo.X.cols(); ++j) {
                file << std::fixed << std::scientific << std::setprecision(2) << mshInfo.X(i, j);
                if (j != mshInfo.X.cols() - 1) file << ",";
            }
            file << "\n";
        }
        file.close();
    } else {
        std::cerr << "无法打开文件" << std::endl;
    }


    std::ofstream fileK("kmatrix.txt");
    if (fileK.is_open()) {
        for (int i = 0; i < mshInfo.kMatrix.rows(); ++i) {
            for (int j = 0; j < mshInfo.kMatrix.cols(); ++j) {
                fileK << std::fixed << std::scientific << std::setprecision(2) << mshInfo.kMatrix(i, j);
                if (j != mshInfo.kMatrix.cols() - 1) fileK << "\t";
            }
            fileK << "\n";
        }
        fileK.close();
    } else {
        std::cerr << "无法打开文件" << std::endl;
    }

    std::ofstream fileF("fmatrix.txt");
    if (fileF.is_open()) {
        for (int i = 0; i < mshInfo.fMatrix.rows(); ++i) {
            for (int j = 0; j < mshInfo.fMatrix.cols(); ++j) {
                fileF << std::fixed << std::scientific <<std::setprecision(2) << mshInfo.fMatrix(i, j);
                if (j != mshInfo.fMatrix.cols() - 1) fileF << ",";
            }
            fileF << "\n";
        }
        fileF.close();
    } else {
        std::cerr << "无法打开文件" << std::endl;
    }

    qDebug() << "calculating finished";
    emit updateProgressBarSignal(100);
    emit enableRenderActionSignal(true);
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

void MainWindow::addGraphics()//添加自定义图形
{
    DialogAddGraphics *dialog = new DialogAddGraphics();
    dialog->setWindowTitle("添加图形");
    QPoint cursorPos = QCursor::pos();
    dialog->move(cursorPos);
    dialog->show();
    connect(dialog, &DialogAddGraphics::sendRectSignal, this, &MainWindow::paintRect);
    connect(dialog, &DialogAddGraphics::sendCircleSignal, this, &MainWindow::paintCircle);
}

void MainWindow::paintRect(Rect rect)
{
    QGraphicsRectItem* rectItem = ui->graphicsView->scene()->addRect(rect.x, rect.y, rect.width, rect.height);
    rectItem->setPen(this->pen);
}

void MainWindow::paintCircle(Circle circle)
{
    QGraphicsEllipseItem * circleItem = ui->graphicsView->scene()->addEllipse(circle.x - circle.radius, circle.y - circle.radius,
                                                                             circle.radius * 2, circle.radius * 2 );
    circleItem->setPen(this->pen);
}

void MainWindow::generateMsh()//生成网格(从输入的图形中生成)
{
    gmsh::initialize();
    gmsh::model::add(mshInfo.filePath.toStdString());
    if (ui->lcValue->text().isEmpty()) {
        QMessageBox::warning(this, "警告", "lc不能为空，请输入有效的内容。");
        return; // 退出当前函数
    }
    // if (!mshInfo.coordinates.empty()) {
    //     QMessageBox::warning(this, "警告", "重复生成。");
    //     return;
    // }
    double lc = ui->lcValue->text().toDouble();
    for (Coordinate point : mshInfo.coordinates) {
        mshInfo.pointIndexs.push_back(gmsh::model::geo::addPoint(point.x, point.y, 0, lc));
    }

    for (int i = 1; i < mshInfo.pointIndexs.size(); i++) {
        mshInfo.lineIndexs.push_back(gmsh::model::geo::addLine(mshInfo.pointIndexs[i - 1], mshInfo.pointIndexs[i]));
    }
    mshInfo.lineIndexs.push_back(gmsh::model::geo::addLine(mshInfo.pointIndexs[mshInfo.pointIndexs.size() - 1], mshInfo.pointIndexs[0]));
    gmsh::model::geo::addCurveLoop(mshInfo.lineIndexs, 1);
    //添加圆
    Circle circle = mshInfo.circle;
    mshInfo.circlePointIndexs.push_back(gmsh::model::geo::addPoint(circle.x - circle.radius, circle.y, 0, lc));
    mshInfo.circlePointIndexs.push_back(gmsh::model::geo::addPoint(circle.x, circle.y, 0, lc));
    mshInfo.circlePointIndexs.push_back(gmsh::model::geo::addPoint(circle.x + circle.radius, circle.y, 0, lc    ));

    int circleLeftIndex = gmsh::model::geo::addCircleArc(mshInfo.circlePointIndexs[0], mshInfo.circlePointIndexs[1], mshInfo.circlePointIndexs[2]);
    int circleRightIndex = gmsh::model::geo::addCircleArc(mshInfo.circlePointIndexs[2], mshInfo.circlePointIndexs[1], mshInfo.circlePointIndexs[0]);


    gmsh::model::geo::addCurveLoop({circleLeftIndex, circleRightIndex}, 2);

    gmsh::model::geo::addPlaneSurface({1, 2}, 1);

    gmsh::model::geo::synchronize();

    gmsh::model::mesh::generate(2);


    //初始化网格点和三角形的信息
    mshInfo.initPointAndTriangleInfo();
    this->paintMsh();
    this->permanentLabel->setText("网格数量为：" + QString::number(mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex].size()) + "网格点数量为:" + QString::number(mshInfo.tagMap.size()));

}


struct pair_hash {
    inline std::size_t operator()(const std::pair<int,int> & v) const {
        return v.first * 31 + v.second;
    }
};


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

void MainWindow::addForces()//添加外力
{
    DialogAddForces * dialog = new DialogAddForces;
    dialog->setWindowTitle("添加外力");
    QPoint cursorPos = QCursor::pos();
    dialog->move(cursorPos);
    dialog->show();
}

void MainWindow::addEdges()//添加边界条件
{
    DialogEdgeAdd * dialog = new DialogEdgeAdd;
    dialog->setWindowTitle("添加边界条件");
    QPoint cursorPos = QCursor::pos();
    dialog->move(cursorPos);
    dialog->show();
    connect(dialog, &DialogEdgeAdd::sendCalcActivate, this, &MainWindow::activateCalc);

}

void MainWindow::calcMatrix()//计算k，f，uv矩阵
{
    ui->progressBar->setRange(0, 0);
    QtConcurrent::run(&MainWindow::calcMatrixConcurrent, this);
}

void MainWindow::render()//渲染
{
    calcStressStrain();
    double min = DBL_MAX;
    double max = -DBL_MAX;

    getExtreme(min , max);
    qDebug() << qSetRealNumberPrecision(2) << scientific << "min: " << min << ", max: " << max;
    ui->lineEditMin->setText(QString::number(min, 'e', 2));
    ui->lineEditMax->setText(QString::number(max, 'e', 2));
    for (MechanicBehavior info: mshInfo.mechanicBehaviors) {
        double stressValue = info.equalStress; // 这里获取当前三角形网格的应力值
        double normalizedStress = (stressValue - min) / (max - min);
        int red=0;
        int green=0;
        int blue=0;
        if(normalizedStress<=0.5){
            red = 255;
            green = 510*normalizedStress;
            blue=255-green;
        }else{
            red = 510 * (1-normalizedStress);
            green = 255;
            blue=510 * (normalizedStress - 0.5);
        }
        QColor color(255-red, 255-green, 255-blue);
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

void MainWindow::saveMsh()//保存网格
{
    if (!mshInfo.filePath.isEmpty()) {
        gmsh::write(mshInfo.filePath.toStdString());
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("保存文件"), "",
                                                    tr("文本文件 (*.msh);;所有文件 (*)"));
    if (!fileName.endsWith(".msh", Qt::CaseInsensitive)) {
        fileName += ".msh";
    }
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // 文件已成功创建
        qDebug() << "文件成功创建";
        file.close(); // 关闭文件
    } else {
        qDebug() << "文件创建失败";
    }
    mshInfo.filePath = fileName;
    gmsh::write(mshInfo.filePath.toStdString());
}

void MainWindow::openMsh()//打开网格
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), "/home", tr("所有文件 (*)"));
    if (!fileName.isEmpty()) {
        gmsh::initialize();
        gmsh::open(fileName.toStdString());
        mshInfo.initPointAndTriangleInfo();
        paintMsh();
        this->permanentLabel->setText("网格数量为：" + QString::number(mshInfo.nodeTagsForTriangle[mshInfo.triangleIndex].size())  + "网格点数量为:" + QString::number(mshInfo.tagMap.size()));
    } else {
        QMessageBox::warning(this, tr("警告"), tr("打开文件失败"));
    }
}

void MainWindow::saveConstraint()//保存约束
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("保存约束"), "",
                                                    tr("文本文件 (*.json);;所有文件 (*)"));
    if (!fileName.endsWith(".json", Qt::CaseInsensitive)) {
        fileName += ".json";
    }
    QJsonArray forcesArray;
    for (const auto &force: mshInfo.forces) {
        QJsonObject forceObject;
        forceObject["x"] = force.x;
        forceObject["y"] = force.y;
        forceObject["xForce"] = force.xForce;
        forceObject["yForce"] = force.yForce;
        forcesArray.append(forceObject);
    }
    QJsonArray edgeInfosArray;
    for (const auto &edgeInfo : mshInfo.edgeInfos) {
        QJsonObject edgeInfoObject;
        edgeInfoObject["x"] = edgeInfo.x;
        edgeInfoObject["y"] = edgeInfo.y;
        edgeInfoObject["xFixed"] = edgeInfo.xFixed;
        edgeInfoObject["yFixed"] = edgeInfo.yFixed;
        edgeInfosArray.append(edgeInfoObject);
    }
    QJsonObject dataObject;
    dataObject["forces"] = forcesArray;
    dataObject["edgeInfos"] = edgeInfosArray;
    QJsonDocument doc(dataObject);
    QString jsonString = doc.toJson(QJsonDocument::Indented);
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonString.toUtf8());
        file.close();
    }
}

void MainWindow::openConstraint()//装载约束条件的数据
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("装载约束"), "/home", tr("所有文件(*)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning("无法打开文件");
            return;
        }
        QByteArray jsonData = file.readAll();
        file.close();
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (!doc.isObject()) {
            qWarning("JSON格式错误");
            return;
        }
        QJsonObject jsonObject = doc.object();
        QJsonArray forcesArray = jsonObject["forces"].toArray();
        for (const QJsonValue &value: forcesArray) {
            QJsonObject obj = value.toObject();
            Force force;
            force.x = obj["x"].toDouble();
            force.y = obj["y"].toDouble();
            force.xForce = obj["xForce"].toDouble();
            force.yForce = obj["yForce"].toDouble();
            mshInfo.forces.push_back(force);
        }
        QJsonArray edgeInfosArray = jsonObject["edgeInfos"].toArray();
        vector<EdgeInfo> edgeInfos;
        for (const QJsonValue &value : edgeInfosArray) {
            QJsonObject obj = value.toObject();
            EdgeInfo edgeInfo;
            edgeInfo.x = obj["x"].toDouble();
            edgeInfo.y = obj["y"].toDouble();
            edgeInfo.xFixed = obj["xFixed"].toBool();
            edgeInfo.yFixed = obj["yFixed"].toBool();
            mshInfo.edgeInfos.push_back(edgeInfo);
        }
        this->calcAction->setEnabled(true);
    } else {
        QMessageBox::warning(this, tr("警告"), tr("打开文件失败"));
    }
}

void MainWindow::handleClick(QPointF  point)//点击网格中的点，然后显示出相关信息
{
    if (this->paintState == true) {
        double x = point.x();
        double y = point.y();
        ui->lineEditPointX->setText(QString::number(x));
        ui->lineEditPointY->setText(QString::number(y));
        MechanicBehavior mechanicBehavior;
        double u;
        double v;
        if (getPointInfos(x, y, mechanicBehavior, u , v)) {
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
    else {
        //进行绘图操作

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
