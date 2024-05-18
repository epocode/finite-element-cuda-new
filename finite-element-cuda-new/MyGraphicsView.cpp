#include "MyGraphicsView.h"
#include <QScrollBar>
#include <QMouseEvent>
MyGraphicsView::MyGraphicsView(QWidget* parent)
    : QGraphicsView(parent) {
    setMode(COMMON);
    // ��ʼ������
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setMouseTracking(true);
    setupDragMode();
    myScene = this->scene();
    this->scale(40, 40);
    this->pen.setWidthF(pen.widthF() / this->transform().m11());
    myScene = new QGraphicsScene;
    this->setScene(myScene);
    this->scale(1, -1);
    //��ʾ����ֵ
    coordinateLabel = new QLabel(this);
    coordinateLabel->move(0, 0); // ���������Ͻ�
    coordinateLabel->setText("X: 0, Y: 0");
    coordinateLabel->setStyleSheet("QLabel { color : black; }");
    coordinateLabel->setFixedSize(100, 30);
}

void MyGraphicsView::wheelEvent(QWheelEvent* event) {
    const double scaleFactor = 1.15; // ���ŵı�������
    QPoint scrollAmount = event->angleDelta();
    if (scrollAmount.y() > 0) {
        // ���Ϲ��������֣��Ŵ�
        scale(scaleFactor, scaleFactor);
    }
    else {
        // ���¹��������֣���С
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }

    // �������ҪĬ�ϵĹ�����Ϊ���򲻵��û���� wheelEvent
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
                if (points.size() > 1) {//�����߶�
                    myScene->addLine(QLineF(points[points.size() - 2], point), this->pen);
                }
            }
            else {
                if (isCloseToFirstPoint(point)) {
                    // �������һ���߶αպ϶����
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
            if (points.size() > 1) {//�����߶�
                myScene->addLine(QLineF(points[points.size() - 2], point), this->pen);
            }
        }
        else {
            if (isCloseToFirstPoint(point)) {
                // �������һ���߶αպ϶����
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
    // ����������ʾ��ǩ
    coordinateLabel->setText(QString("X: %1, Y: %2").arg(point.x()).arg(point.y()));
    QGraphicsView::mouseMoveEvent(event); // ���û���ķ���
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
                // ����ҵ���һ���ӽ��ĵ㣬����
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
    emit doubleClicked(scenePoint); // �����ź�
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
    const double snapDistance = 1; // ����������ֵ
    QPointF point = points.first();
    double distance = std::hypot(mousePos.x() - point.x(), mousePos.y() - point.y());
    if (distance < snapDistance) {
        return true;
    }

    return false;
}