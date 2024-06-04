#include "CommonOperator.h"

CommonOperator::CommonOperator(MyGraphicsView* view)
{
	this->view = view;
}

void CommonOperator::handleCoordinateInput(QString text)
{
	
}

void CommonOperator::mousePressEvent(QMouseEvent* event)
{
	QPointF point = view->mapToScene(event->pos());
	if (event->button() == Qt::LeftButton) {
		view->lastMousePos = event->pos();
	}
}

void CommonOperator::mouseMoveEvent(QMouseEvent* event)
{
	QPointF point = view->mapToScene(event->pos());
	if (event->buttons() & Qt::LeftButton) {
		QPointF delta = view->mapToScene(event->pos()) - view->mapToScene(view->lastMousePos);
		view->centerOn(view->mapToScene(view->viewport()->rect().center()) - delta);
		view->lastMousePos = event->pos();
	}

}

void CommonOperator::keyPressEvent(QKeyEvent* event)
{

}
	