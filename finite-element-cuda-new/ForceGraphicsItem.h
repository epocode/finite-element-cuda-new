#pragma once

#include <QGraphicsObject>
#include <QPen>
class ForceGraphicsItem  : public QGraphicsObject
{
	Q_OBJECT

public:
	ForceGraphicsItem(QGraphicsItem *parent, double x, double y, double xForce, double yForce, QPen pen);
	~ForceGraphicsItem();
	double x;
	double y;
	double xForce;
	double yForce;
	QPen myPen;
protected:
	QRectF boundingRect() const override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};
