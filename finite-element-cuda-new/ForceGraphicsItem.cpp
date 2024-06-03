#include "ForceGraphicsItem.h"
#include <QPainter>
#include <cmath>

ForceGraphicsItem::ForceGraphicsItem(QGraphicsItem* parent, double x, double y, double xForce, double yForce, QPen pen) 
	: QGraphicsObject(parent)
{
	this->x = x;
	this->y = y;
	this->xForce = xForce;
	this->yForce = yForce;
	setScale(0.5);
	this->myPen = pen;
	myPen.setWidth(0.5);
	myPen.setColor(Qt::red);
	setTransformOriginPoint(QPointF(1, 2));
	setPos(x-1, y-2);
	double theta_radians = std::atan2(yForce, xForce);
	double theta_degrees = theta_radians * (180.0 / M_PI);
	setRotation(theta_degrees + 270);
	this->setZValue(1);
}

ForceGraphicsItem::~ForceGraphicsItem()
{}

QRectF ForceGraphicsItem::boundingRect() const
{
	return QRectF(0, 0, 2, 2);
}

void ForceGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	painter->setPen(myPen);
	painter->drawLine(1, 2, 1, 0); // ¼ýÉí
	painter->drawLine(1, 2, 0, 1); // ¼ýÍ·×ó°ë²¿
	painter->drawLine(1, 2, 2, 1);
	
}
