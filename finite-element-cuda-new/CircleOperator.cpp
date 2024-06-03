#include "CircleOperator.h"

CircleOperator::CircleOperator(MyGraphicsView* view)
{
	this->view = view;
}

void CircleOperator::handleCoordinateInput(QString text)
{
    QStringList list = text.split(" ");
    double x = list.at(0).toDouble();
    double y = list.at(1).toDouble();
    QPointF point(x, y);
    list = text.split(" ");
    if (! view->isDrawing) {
         view->startPoint = point;
         view->isDrawing = true;
        emit  view->setTipsSignal(QString("设置终点坐标 x y:"));
    }
    else {
        QPointF endPoint = point;
        qreal radius = QLineF( view->startPoint, endPoint).length();
        if (view->tempEllipse) {
             view->myScene->removeItem((QGraphicsItem*)view->tempEllipse);
            delete view->tempEllipse;
             view->myScene->addEllipse( view->startPoint.x() - radius,  view->startPoint.y() - radius, 2 * radius, 2 * radius, view->pen);
        }
        else {
            view->tempEllipse =  view->myScene->addEllipse( view->startPoint.x() - radius,  view->startPoint.y() - radius, 2 * radius, 2 * radius, view->pen);
        }
        view->isDrawing = false;
        view->tempEllipse = nullptr;
        emit view->createCircleSignal( view->startPoint.x(),  view->startPoint.y(), radius);
        emit view->resetInputAreaSignal();
    }
}

void CircleOperator::mousePressEvent(QMouseEvent* event)
{
    QPointF point = view->mapToScene(event->pos());
    if (event->button() == Qt::LeftButton) {
        if (! view->isDrawing) {
             view->startPoint =  view->mapToScene(event->pos());
             view->isDrawing = true;
            emit  view->setTipsSignal(QString("设置终点坐标 x y:"));
        }
        else {
            QPointF endPoint =  view->mapToScene(event->pos());
            qreal radius = QLineF( view->startPoint, endPoint).length();
            if (view->tempEllipse) {
                 view->myScene->removeItem((QGraphicsItem*)view->tempEllipse);
                delete view->tempEllipse;
                 view->myScene->addEllipse( view->startPoint.x() - radius,  view->startPoint.y() - radius, 2 * radius, 2 * radius, view->pen);
            }
            else {
                view->tempEllipse =  view->myScene->addEllipse( view->startPoint.x() - radius,  view->startPoint.y() - radius, 2 * radius, 2 * radius, view->pen);
            }
             view->isDrawing = false;
            view->tempEllipse = nullptr;
            emit view->createCircleSignal( view->startPoint.x(),  view->startPoint.y(), radius);
            emit view->resetInputAreaSignal();
             view->setMode(view->COMMON);
        }
    }
}

void CircleOperator::mouseMoveEvent(QMouseEvent* event)
{
    QPointF point =  view->mapToScene(event->pos());
    // 更新坐标显示标签
    view->coordinateLabel->setText(QString("X: %1, Y: %2").arg(point.x()).arg(point.y()));
    if ( view->isDrawing) {
        QPointF endPoint =  view->mapToScene(event->pos());
        qreal radius = QLineF( view->startPoint, endPoint).length();
        if (view->tempEllipse) {
             view->myScene->removeItem((QGraphicsItem*)view->tempEllipse);
            delete view->tempEllipse;
            view->tempEllipse =  view->myScene->addEllipse( view->startPoint.x() - radius,  view->startPoint.y() - radius, 2 * radius, 2 * radius, view->pen);
        }
        else {
            view->tempEllipse =  view->myScene->addEllipse( view->startPoint.x() - radius,  view->startPoint.y() - radius, 2 * radius, 2 * radius, view->pen);
        }
    }
}

void CircleOperator::keyPressEvent(QKeyEvent* event)
{
    if (view->tempEllipse) {
         view->myScene->removeItem((QGraphicsItem*)view->tempEllipse);
        delete view->tempEllipse;
    }
    view->tempEllipse = nullptr;
     view->isDrawing = false;
    emit view->resetInputAreaSignal();
     view->setMode(view->COMMON);
}
