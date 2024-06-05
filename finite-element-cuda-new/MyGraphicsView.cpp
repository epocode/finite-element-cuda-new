#include "MyGraphicsView.h"
#include <QScrollBar>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QString>
#include "MyGradientLabel.h"
#include <qgraphicsproxywidget.h>
#include <QGroupBox>
#include <QVBoxLayout>
#include "ConcentratedForceOperator.h"
#include "UniformForceOperator.h"
#include "AddConstraintOperator.h"
#include "ForceGraphicsItem.h"
#include "InfiniteGraphicsScene.h"
MyGraphicsView::MyGraphicsView(QWidget* parent)
    : QGraphicsView(parent) {
    // 初始化代码
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setMouseTracking(true);
    //setDragMode(QGraphicsView::ScrollHandDrag); 
    this->scale(40, 40);
    this->pen.setWidthF(pen.widthF() / this->transform().m11());
    myScene = new InfiniteGraphicsScene(this, pen);
    this->setScene(myScene);
    this->scale(1, -1);
    //显示坐标值
    coordinateLabel = new QLabel(this);
    coordinateLabel->move(0, 0); // 放置在左上角
    coordinateLabel->setText("X: 0, Y: 0");
    coordinateLabel->setStyleSheet("QLabel { color : yellow; }");
    coordinateLabel->setFixedSize(100, 30);
    isDrawing = false;
    //添加颜色条
    gradientBox = new QGroupBox(this);
    QVBoxLayout* layout= new QVBoxLayout();
    maxGradientLabel = new QLabel(gradientBox);
    maxGradientLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    colorBar = new MyGradientLabel(gradientBox);
    minGradientLabel = new QLabel(gradientBox);
    minGradientLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    maxGradientLabel->setStyleSheet("color: black;");
    minGradientLabel->setStyleSheet("color: black;");
    layout->addWidget(maxGradientLabel);
    layout->addWidget(colorBar);
    layout->addWidget(minGradientLabel);
    gradientBox->setLayout(layout);
    gradientBox->move(0, 30);
    gradientBox->setFixedWidth(80);
    gradientBox->hide();
    //设置缩放倍数的数值显示
    scaleLabel = new QLabel("100%", this);
    scaleLabel->move(200, 0);
    scaleLabel->setMargin(5);
    scaleLabel->setStyleSheet("QLabel{ color: yellow;}");
    scaleLabel->setFixedSize(50, 30);
    //设置绘图区状态
    operatorList.push_back(new CommonOperator(this));
    operatorList.push_back(new PolygonOperator(this));
    operatorList.push_back(new RectOperator(this));
    operatorList.push_back(new CircleOperator(this));
    operatorList.push_back(new ConcentratedForceOperator(this));
    operatorList.push_back(new UniformForceOperator(this));
    operatorList.push_back(new AddConstraintOperator(this));
    setMode(COMMON);

    this->setRenderHint(QPainter::Antialiasing);
    this->setRenderHint(QPainter::SmoothPixmapTransform);
    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
}

void MyGraphicsView::setMode(Mode mode) {
    this->myOperator = this->operatorList[(int)mode];
}

void MyGraphicsView::setMode(QString mode)
{
    if (mode == QString("COMMON")) {
        setMode(COMMON);
    }
    else if (mode == QString("CREATELINE")) {
        setMode(CREATELINE);
    }
    else if (mode == QString("CREATERECT")) {
        setMode(CREATERECT);
    }
    else if (mode == QString("CREATECIRCLE")) {
        setMode(CREATECIRCLE);
    }
    else if (mode == QString("CONCENTRATEFORCE")) {
        setMode(CONCENTRATEDFORCE);
    }
    else if (mode == QString("UNIFORMFORCE")) {
        setMode(UNIFORMFORCE);
    }
    else if (mode == QString("CONSTRAINT")) {
        setMode(CONSTRAINT);
    }
}


void MyGraphicsView::wheelEvent(QWheelEvent* event) {
    const double scaleFactor = 1.15; // 缩放的比例因子
    QPoint scrollAmount = event->angleDelta();
    if (scrollAmount.y() > 0) {
        scale(scaleFactor, scaleFactor);
    }
    else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
    qreal currentScaleFactor = this->transform().m11();
    int zoomPercentage = static_cast<int>(currentScaleFactor * 100);
    scaleLabel->setText(QString::number(zoomPercentage) + "%");
}



void MyGraphicsView::handleCoordinateInput(QString text)
{
    myOperator->handleCoordinateInput(text);
}


void MyGraphicsView::mousePressEvent(QMouseEvent* event)
{
    myOperator->mousePressEvent(event);
    QGraphicsView::mousePressEvent(event);
}


void MyGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    QPointF point = mapToScene(event->pos());
    // 更新坐标显示标签
    coordinateLabel->setText(QString("X: %1, Y: %2")
        .arg(point.x(), 0, 'f', 1)
        .arg(point.y(), 0, 'f', 1));

    myOperator->mouseMoveEvent(event);
    QGraphicsView::mouseMoveEvent(event);
}


void MyGraphicsView::mouseReleaseEvent(QMouseEvent * event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

void MyGraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QPointF scenePoint = mapToScene(event->pos());
    emit doubleClicked(scenePoint); // 发送信号
}

bool MyGraphicsView::isCloseToFirstPoint(const QPointF& mousePos)
{
    const double snapDistance = 1; // 吸附距离阈值
    QPointF point = points.first();
    double distance = std::hypot(mousePos.x() - point.x(), mousePos.y() - point.y());
    if (distance < snapDistance) {
        return true;
    }

    return false;
}

void MyGraphicsView::keyPressEvent(QKeyEvent* event) {
    
    if (event->key() == Qt::Key_Escape) {
        myOperator->keyPressEvent(event);
    }
    else {
        // 如果按下的不是 ESC 键，则调用基类的方法处理该事件
        QGraphicsView::keyPressEvent(event);
    }
}


void MyGraphicsView::hideRenderInfo()
{
    this->gradientBox->hide();
}

void MyGraphicsView::showRenderInfo(double max, double min) {
    this->gradientBox->show();
    this->maxGradientLabel->setText(QString::number(max, 'e', 1));
    this->minGradientLabel->setText(QString::number(min, 'e', 1));
}

void MyGraphicsView::handleDirectForceInput(Force force) {
    double x = force.x;
    double y = force.y;
    double xForce = force.xForce;
    double yForce = force.yForce;
    ForceGraphicsItem* arrow = new ForceGraphicsItem(nullptr, x, y, xForce, yForce, this->pen);
    this->myScene->addItem(arrow);
    emit addConcentratedForceSignal(x, y, xForce, yForce);
}


void MyGraphicsView::handleDirectConstraintInput(EdgeInfo edgeInfo) {
    double x = edgeInfo.x;
    double y = edgeInfo.y;
    double xFixed = edgeInfo.xFixed;
    double yFixed = edgeInfo.yFixed;
    QPen tempPen = this->pen;
    tempPen.setColor(Qt::yellow);
    QGraphicsEllipseItem* graphicsItem = this->myScene->addEllipse(x - 0.1, y - 0.1, 0.2, 0.2, tempPen);
    graphicsItem->setZValue(1);
    QString msg = "成功添加约束，x:" + QString::number(x) + ", y:" + QString::number(y);
    if (xFixed) {
        msg += ", 水平方向设置了约束";
    }
    if (yFixed) {
        msg += ", 竖直方向设置了约束";
    }
    emit addConstraintSignal(msg);
}

void MyGraphicsView::paintByPixel(QPointF point) {
    QImage image(0.01, 0.01, QImage::Format_RGB32);
    image.setPixel(0, 0, qRgb(255, 0, 0));
    QPixmap pixmap = QPixmap::fromImage(image);
    QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(pixmap);
    pixmapItem->setPos(point.x(), point.y());
    myScene->addItem(pixmapItem);
}