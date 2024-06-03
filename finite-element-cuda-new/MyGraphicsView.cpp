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
MyGraphicsView::MyGraphicsView(QWidget* parent)
    : QGraphicsView(parent) {
    // 初始化代码
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setMouseTracking(true);
    setupDragMode();
    this->scale(40, 40);
    this->pen.setWidthF(pen.widthF() / this->transform().m11());
    myScene = new QGraphicsScene;
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
    gradientBox->move(0, 40);
    gradientBox->hide();
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
        // 向上滚动鼠标滚轮，放大
        scale(scaleFactor, scaleFactor);
    }
    else {
        // 向下滚动鼠标滚轮，缩小
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}



void MyGraphicsView::handleCoordinateInput(QString text)
{
    myOperator->handleCoordinateInput(text);
}


void MyGraphicsView::mousePressEvent(QMouseEvent* event)
{
    myOperator->mousePressEvent(event);
}


void MyGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    myOperator->mouseMoveEvent(event);
}


    void MyGraphicsView::mouseReleaseEvent(QMouseEvent * event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

void MyGraphicsView::setupDragMode()
{
    setDragMode(QGraphicsView::ScrollHandDrag);
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


