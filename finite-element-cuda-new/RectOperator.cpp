#include "RectOperator.h"
#include <QGraphicsRectItem>
RectOperator::RectOperator(MyGraphicsView* view)
{
	this->view = view;
}

void RectOperator::handleCoordinateInput(QString text)
{
    QStringList list = text.split(" ");
    double x = list.at(0).toDouble();
    double y = list.at(1).toDouble();
    QPointF point(x, y);
    list = text.split(" ");
    x = list.at(0).toDouble();
    y = list.at(1).toDouble();
    point = QPointF(x, y);
    if (view->isDrawing) {//完成了创建
        if (view->tempRect) {
            view->myScene->removeItem((QGraphicsItem*)(view->tempRect));
            delete view->tempRect;
        }
        view->tempRect = view->myScene->addRect(QRectF(view->rectStartPoint, point),  view->pen);
        view->tempRect = nullptr;
        view->isDrawing = false;
        view->setMode( view->COMMON);
        emit  view->createRectSignal( view->rectStartPoint, point);
        emit view->resetInputAreaSignal();
    }
    else {
         view->isDrawing = true;
         view->rectStartPoint = point;
         emit view->setTipsSignal(QString("设置对顶角顶点坐标 x y:"));
    }
}

void RectOperator::mousePressEvent(QMouseEvent* event)
{
    QPointF point =  view->mapToScene(event->pos());
    if (event->button() == Qt::LeftButton) {
        if (view->isDrawing) {//完成了创建
            if (view->tempRect) {
                view->myScene->removeItem((QGraphicsItem*)(view->tempRect));
                delete view->tempRect;
            }
            view->tempRect = view->myScene->addRect(QRectF( view->rectStartPoint, point),  view->pen);
            view->tempRect = nullptr;
            view->isDrawing = false;
            view->setMode( view->COMMON);
            emit  view->createRectSignal( view->rectStartPoint, point);
            emit view->resetInputAreaSignal();
        }
        else {
             view->isDrawing = true;
             view->rectStartPoint = point;
            // 创建一个临时的矩形来显示预创建的形状
            view->tempRect = view->myScene->addRect(QRectF( view->rectStartPoint, point),  view->pen);
            emit view->setTipsSignal(QString("设置对顶角顶点坐标 x y:"));
        }
    }
}

void RectOperator::mouseMoveEvent(QMouseEvent* event)
{
    QPointF point =  view->mapToScene(event->pos());
    // 更新坐标显示标签
    view->coordinateLabel->setText(QString("X: %1, Y: %2").arg(point.x()).arg(point.y()));
    if ( view->isDrawing) {
        if (view->tempRect) {//完成了创建
            view->myScene->removeItem((QGraphicsItem*)(view->tempRect));
            delete view->tempRect;
            view->tempRect = view->myScene->addRect(QRectF( view->rectStartPoint, point),  view->pen);

        }
        else {
            view->tempRect = new QGraphicsRectItem(QRectF( view->rectStartPoint, point));
            view->tempRect = view->myScene->addRect(QRectF( view->rectStartPoint, point),  view->pen);
        }
    }
}

void RectOperator::keyPressEvent(QKeyEvent* event)
{
    if (view->tempRect) {//完成了创建
        view->myScene->removeItem((QGraphicsItem*)(view->tempRect));
        delete view->tempRect;
    }
    view->tempRect = nullptr;
     view->isDrawing = false;
     view->setMode( view->COMMON);
    emit view->resetInputAreaSignal();
}
