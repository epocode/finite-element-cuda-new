#include "UniformForceOperator.h"
#include "UniformForceGraphicsItem.h"
#include <QInputDialog>
#include "Controller.h"
UniformForceOperator::UniformForceOperator(MyGraphicsView* view)
{
	this->view = view;
	isFirst = true;
}

void UniformForceOperator::handleCoordinateInput(QString text)
{
	QStringList list = text.split(" ");
	double x = list.at(0).toDouble();
	double y = list.at(1).toDouble();
	if (isFirst) {
		startX = x;
		startY = y;
		tempItem = view->myScene->addEllipse(startX - 1, startY - 1, 2, 2, view->pen);
		isFirst = false;
		emit view->setTipsSignal(QString("设置终点坐标 x y:"));
	}
	else {
		double endX = x;
		double endY = y;
		handleInput(endX, endY);
	}
}

void UniformForceOperator::mousePressEvent(QMouseEvent* event)
{
	QPointF point = view->mapToScene(event->pos());
	if (isFirst) {
		startX = point.x();
		startY = point.y();
		tempItem = view->myScene->addEllipse(startX - 0.1, startY - 0.1, 0.2, 0.2, view->pen);
		isFirst = false;
		emit view->setTipsSignal(QString("设置终点坐标 x y:"));
	}
	else {
		double endX = point.x();
		double endY = point.y();
		handleInput(endX, endY);
	}

}

void UniformForceOperator::mouseMoveEvent(QMouseEvent* event)
{
	QPointF point = view->mapToScene(event->pos());
}

void UniformForceOperator::keyPressEvent(QKeyEvent* event)
{
}

void UniformForceOperator::handleInput(double endX, double endY)
{
	bool okx;
	bool oky;
	double xForce = (QInputDialog::getText(view, "输入力的大小", "水平方向均布力:", QLineEdit::Normal, "", &okx)).toDouble();
	double yForce = (QInputDialog::getText(view, "输入力的大小", "竖直方向均布力：", QLineEdit::Normal, "", &oky)).toDouble();
	if (!okx || !oky) {
		view->setMode("COMMON");
		emit view->resetInputAreaSignal();
		view->myScene->removeItem((QGraphicsItem*)tempItem);
		if (tempItem) {
			delete tempItem;
		}
		this->isFirst = true;
		return;
	}
	view->setMode("COMMON");
	emit view->resetInputAreaSignal();
	UniformForceGraphicsItem* forceItem = new UniformForceGraphicsItem(nullptr, startX, startY, endX, endY, xForce, yForce, view->pen);
	view->myScene->addItem(forceItem);
	Controller::addUniformLoad(startX, startY, endX, endY, xForce, yForce);
	this->isFirst = true;
	view->setMode("COMMON");
	emit view->resetInputAreaSignal();
	view->myScene->removeItem((QGraphicsItem*)tempItem);
	delete tempItem;
	emit view->addUniformForceSignal(startX, startY, endX, endY, xForce, yForce);
}
