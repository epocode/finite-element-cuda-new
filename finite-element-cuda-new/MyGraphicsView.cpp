#include "MyGraphicsView.h"
#include <QScrollBar>
#include <QMouseEvent>
MyGraphicsView::MyGraphicsView(QWidget* parent)
    : QGraphicsView(parent) {
    setMode(COMMON);
    // 初始化代码
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setMouseTracking(true);
    setupDragMode();
    myScene = this->scene();
    this->scale(40, 40);
    this->pen.setWidthF(pen.widthF() / this->transform().m11());
    myScene = new QGraphicsScene;
    this->setScene(myScene);
    this->scale(1, -1);
    //显示坐标值
    coordinateLabel = new QLabel(this);
    coordinateLabel->move(0, 0); // 放置在左上角
    coordinateLabel->setText("X: 0, Y: 0");
    coordinateLabel->setStyleSheet("QLabel { color : black; }");
    coordinateLabel->setFixedSize(100, 30);
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

    // 如果不需要默认的滚动行为，则不调用基类的 wheelEvent
    // QGraphicsView::wheelEvent(event);
}





void MyGraphicsView::mousePressEvent(QMouseEvent* event)
{
    switch (curMode) {
    case COMMON:
        if (event->button() == Qt::LeftButton) {
            lastMousePos = event->pos();
        }
        QGraphicsView::mousePressEvent(event);
        break;
    case CREATELINE:
        QPointF point = mapToScene(event->pos());
        if (event->button() == Qt::MouseButton::LeftButton) {
            if (points.size() <= 2) {
                points.append(point);
                myScene->addEllipse(point.x() - 0.05, point.y() - 0.05, 0.1, 0.1, this->pen);
                if (points.size() > 1) {//绘制线段
                    myScene->addLine(QLineF(points[points.size() - 2], point), this->pen);
                }
            }
            else {
                if (isCloseToFirstPoint(point)) {
                    // 绘制最后一条线段闭合多边形
                    myScene->addLine(QLineF(points.last(), points.first()), this->pen);
                    setMode(COMMON);
                    emit createPolygonSignal(points);
                    points.clear();
                }
                else {
                    points.append(point);
                    myScene->addEllipse(point.x() - 0.05, point.y() - 0.05, 0.1, 0.1, this->pen);
                    myScene->addLine(QLineF(points[points.size() - 2], point), this->pen);
                }
            }
        }
        break;
    }
}

void MyGraphicsView::handleCoordinateInput(QString text)
{
    QStringList list = text.split(" ");
    bool okX, okY;
    double x = list.at(0).toDouble(&okX);
    double y = list.at(1).toDouble(&okY);
    if (okX && okY) {
        QPointF point(x, y);
        if (points.size() <= 2) {
            points.append(point);
            myScene->addEllipse(point.x() - 0.05, point.y() - 0.05, 0.1, 0.1, this->pen);
            if (points.size() > 1) {//绘制线段
                myScene->addLine(QLineF(points[points.size() - 2], point), this->pen);
            }
        }
        else {
            if (isCloseToFirstPoint(point)) {
                // 绘制最后一条线段闭合多边形
                myScene->addLine(QLineF(points.last(), points.first()), this->pen);
                setMode(COMMON);
                emit createPolygonSignal(points);
                points.clear();
            }
            else {
                points.append(point);
                myScene->addEllipse(point.x() - 0.05, point.y() - 0.05, 0.1, 0.1, this->pen);
                myScene->addLine(QLineF(points[points.size() - 2], point), this->pen);
            }
        }
    }
    else {
        return;
    }
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    QPointF point = mapToScene(event->pos());
    // 更新坐标显示标签
    coordinateLabel->setText(QString("X: %1, Y: %2").arg(point.x()).arg(point.y()));
    QGraphicsView::mouseMoveEvent(event); // 调用基类的方法
    switch (curMode) {
    case COMMON:
        if (event->buttons() & Qt::LeftButton) {
            QPointF delta = mapToScene(event->pos()) - mapToScene(lastMousePos);
            translate(delta.x(), delta.y());
            lastMousePos = event->pos();
        }
        QGraphicsView::mouseMoveEvent(event);
        break;
    case CREATELINE:
        if (!points.isEmpty()) {
            QPointF point = mapToScene(event->pos());
            bool found = isCloseToFirstPoint(point);
            if (found) {
                // 如果找到了一个接近的点，吸附
                if (tempLine) {
                    myScene->removeItem((QGraphicsItem*)(tempLine));
                    delete tempLine;
                }
                tempLine = myScene->addLine(QLineF(points.last(), points.first()), this->pen);
            }
            else {
                if (tempLine) {
                    myScene->removeItem((QGraphicsItem*)(tempLine));
                    delete tempLine;
                }
                tempLine = myScene->addLine(QLineF(points.last(), point), this->pen);
            }

        }
    }
    
}

void MyGraphicsView::mouseReleaseEvent(QMouseEvent* event)
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





void MyGraphicsView::setMode(Mode mode) {
    this->curMode = mode;
}

void MyGraphicsView::setMode(QString mode)
{
    if (mode == QString("COMMON")) {
        setMode(COMMON);
    }
    else if (mode == QString("CREATELINE")) {
        setMode(CREATELINE);
    }
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