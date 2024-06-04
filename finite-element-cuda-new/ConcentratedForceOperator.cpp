#include "ConcentratedForceOperator.h"
#include <QInputDialog>
#include <ForceGraphicsItem.h>
#include <Controller.h>
ConcentratedForceOperator::ConcentratedForceOperator(MyGraphicsView* view)
{
    this->view = view;
}

void ConcentratedForceOperator::handleCoordinateInput(QString text)
{
    QStringList list = text.split(" ");
    double x = list.at(0).toDouble();
    double y = list.at(1).toDouble();
    handleInput(x, y);
}

void ConcentratedForceOperator::mousePressEvent(QMouseEvent* event)
{
	QPointF point = view->mapToScene(event->pos());
    handleInput(point.x(), point.y());
}

void ConcentratedForceOperator::mouseMoveEvent(QMouseEvent* event)
{
    QPointF point = view->mapToScene(event->pos());
}

void ConcentratedForceOperator::keyPressEvent(QKeyEvent* event)
{

}

void ConcentratedForceOperator::handleInput(double x, double y)
{
    bool okx;
    bool oky;
    double xForce = (QInputDialog::getText(view, "输入力的大小", "水平方向作用力:", QLineEdit::Normal, "", &okx)).toDouble();
    double yForce = (QInputDialog::getText(view, "输入力的大小", "竖直方向作用力：", QLineEdit::Normal, "", &oky)).toDouble();
    if (!okx || !oky) {
        view->setMode("COMMON");
        emit view->resetInputAreaSignal();
        return;
    }
    ForceGraphicsItem* arrow = new ForceGraphicsItem(nullptr, x, y, xForce, yForce, view->pen);
    view->myScene->addItem(arrow);
    Controller::addConcentratedForce(x, y, xForce, yForce);
    view->setMode("COMMON");
    emit view->resetInputAreaSignal();
    emit view->addConcentratedForceSignal(x, y, xForce, yForce);
}
