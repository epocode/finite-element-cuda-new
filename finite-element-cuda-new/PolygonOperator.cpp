#include "PolygonOperator.h"

PolygonOperator::PolygonOperator(MyGraphicsView* view)
{
	this->view = view;
}

void PolygonOperator::handleCoordinateInput(QString text)
{
    QStringList list = text.split(" ");
    double x = list.at(0).toDouble();
    double y = list.at(1).toDouble();
    QPointF point(x, y);
    if (view->points.size() <= 2) {
        view->points.append(point);
        view->myScene->addEllipse(point.x() - 0.05, point.y() - 0.05, 0.1, 0.1, view->pen);
        emit view->setTipsSignal(QString("设置下一个点的坐标 x y:"));
        if (view->points.size() > 1) {//绘制线段
            view->myScene->addLine(QLineF(view->points[view->points.size() - 2], point), view->pen);
        }
    }
    else {
        if (view->isCloseToFirstPoint(point)) {
            // 绘制最后一条线段闭合多边形
            view->myScene->addLine(QLineF(view->points.last(), view->points.first()), view->pen);
            view->setMode(view->COMMON);
            emit view->createPolygonSignal(view->points);
            view->points.clear();
            view->tempLine = nullptr;
        }
        else {
            view->points.append(point);
            view->myScene->addEllipse(point.x() - 0.05, point.y() - 0.05, 0.1, 0.1, view->pen);
            view->myScene->addLine(QLineF(view->points[view->points.size() - 2], point), view->pen);
        }
    }
}

void PolygonOperator::mousePressEvent(QMouseEvent* event)
{
    QPointF point = view->mapToScene(event->pos());
    if (event->button() == Qt::MouseButton::LeftButton) {
        if (view->points.size() <= 2) {
            view->points.append(point);
            view->ellipseItemList.push_back(view->myScene->addEllipse(point.x() - 0.05, point.y() - 0.05, 0.1, 0.1, view->pen));
            if (view->points.size() > 1) {//绘制线段
                view->lineItems.push_back(view->myScene->addLine(QLineF(view->points[view->points.size() - 2], point), view->pen));
            }
            QString msg = QString("设置下一个点的坐标 x y : ");
            emit view->setTipsSignal(msg);
        }
        else {
            if (view->isCloseToFirstPoint(point)) {
                // 绘制最后一条线段闭合多边形
                view->lineItems.push_back(view->myScene->addLine(QLineF(view->points.last(), view->points.first()), view->pen));
                view->setMode(view->COMMON);
                emit view->createPolygonSignal(view->points);
                emit view->resetInputAreaSignal();
                view->points.clear();
                view->lineItems.clear();
                view->ellipseItemList.clear();
            }
            else {
                view->points.append(point);
                view->ellipseItemList.push_back(view->myScene->addEllipse(point.x() - 0.05, point.y() - 0.05, 0.1, 0.1, view->pen));
                view->lineItems.push_back(view->myScene->addLine(QLineF(view->points[view->points.size() - 2], point), view->pen));
            }
        }
    }
}

void PolygonOperator::mouseMoveEvent(QMouseEvent* event)
{
    QPointF point = view->mapToScene(event->pos());
    // 更新坐标显示标签
    view->coordinateLabel->setText(QString("X: %1, Y: %2").arg(point.x()).arg(point.y()));
    if (!view->points.isEmpty()) {
            QPointF point = view->mapToScene(event->pos());
            bool found = view->isCloseToFirstPoint(point);
            if (found) {
                // 与首个点连接上后
                if (view->tempLine) {
                    view->myScene->removeItem((QGraphicsItem*)(view->tempLine));
                    delete view->tempLine;
                }
                view->tempLine = view->myScene->addLine(QLineF(view->points.last(), view->points.first()), view->pen);
            }
            else {
                if (view->tempLine) {
                    view->myScene->removeItem((QGraphicsItem*)(view->tempLine));
                    delete view->tempLine;
                }
                view->tempLine = view->myScene->addLine(QLineF(view->points.last(), point), view->pen);
            }

        }
}

void PolygonOperator::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        if (view->points.size() == 0) {
            view->setMode(view->COMMON);
            emit view->resetInputAreaSignal();
            view->tempLine = nullptr;
        }
        if (view->points.size() == 1) {
            if (view->tempLine) {
                view->myScene->removeItem((QGraphicsItem*)view->tempLine);
                delete view->tempLine;
            }
            view->points.removeLast();
            QGraphicsEllipseItem* ellipseItem = view->ellipseItemList.last();
            view->ellipseItemList.removeLast();
            view->myScene->removeItem((QGraphicsItem*)ellipseItem);
            delete ellipseItem;
            view->setMode(view->COMMON);
            emit view->resetInputAreaSignal();
            view->tempLine = nullptr;
        }
        else {
            if (view->tempLine) {
                view->myScene->removeItem((QGraphicsItem*)view->tempLine);
                delete view->tempLine;
            }
            view->tempLine = view->lineItems.last();
            view->lineItems.removeLast();
            view->myScene->removeItem((QGraphicsItem*)view->tempLine);
            delete view->tempLine;
            QGraphicsEllipseItem* ellipseItem = view->ellipseItemList.last();
            view->ellipseItemList.removeLast();
            view->myScene->removeItem((QGraphicsItem*)ellipseItem);
            delete ellipseItem;
            view->points.removeLast();
            view->tempLine = nullptr;
        }
    }
}
