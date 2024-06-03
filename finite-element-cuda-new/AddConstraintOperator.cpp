#include "AddConstraintOperator.h"
#include <QInputDialog>
#include "Controller.h"
#include <QGraphicsEllipseItem>
#include <QPen>
AddConstraintOperator::AddConstraintOperator(MyGraphicsView* view)
{
	this->view = view;
}

void AddConstraintOperator::handleCoordinateInput(QString text)
{
    QStringList list = text.split(" ");
    double x = list.at(0).toDouble();
    double y = list.at(1).toDouble();
    QPointF point(x, y);
    handleInput(point);
}

void AddConstraintOperator::mousePressEvent(QMouseEvent* event)
{
    QPointF point = view->mapToScene(event->pos());
    handleInput(point);
}

void AddConstraintOperator::mouseMoveEvent(QMouseEvent* event)
{
    QPointF point = view->mapToScene(event->pos());
    // 更新坐标显示标签
    view->coordinateLabel->setText(QString("X: %1, Y: %2").arg(point.x()).arg(point.y()));
}

void AddConstraintOperator::keyPressEvent(QKeyEvent* event)
{

}

void AddConstraintOperator::handleInput(QPointF point)
{
    double x = point.x();
    double y = point.y();
    bool ok;
    QStringList items = { "仅限制x方向", "仅限制y方向", "同时限制x和y方向" };
    QString item = QInputDialog::getItem(view, "约束限定方式", "选择该点的约束限制方向:", items, 0, false, &ok);
    if (!ok || item.isEmpty()) {
        view->setMode(view->COMMON);
        emit view->resetInputAreaSignal();
        return;
    }
    bool xFixed = false;
    bool yFixed = false;
    if (item == items[0]) {
        xFixed = true;
        yFixed = false;
    }
    else if (item == items[1]) {
        xFixed = false;
        yFixed = true;
    }
    else {
        xFixed = true;
        yFixed = true;
    }
    QPen myPen = view->pen;
    myPen.setColor(Qt::yellow);
    QGraphicsEllipseItem* graphicsItem = view->myScene->addEllipse(x - 0.1, y - 0.1, 0.2, 0.2, myPen);
    graphicsItem->setZValue(1);
    Controller::addEdges(x, y, xFixed, yFixed);
    emit view->sendCalcActivate();
    view->setMode(view->COMMON);
    emit view->resetInputAreaSignal();
    QString msg = "成功添加约束，x:" + QString::number(x) + ", y:" + QString::number(y);
    if (xFixed) {
        msg += ", 水平方向设置了约束";
    }
    if (yFixed) {
        msg += ", 竖直方向设置了约束";
    }
    emit view->addConstraintSignal(msg);
}
