#pragma once
#include <QGraphicsObject>
#include <QPen>
class UniformForceGraphicsItem :
    public QGraphicsObject
{
    Q_OBJECT
public:
	UniformForceGraphicsItem(QGraphicsItem* parent, double startX, double startY, double endX, double endY
		, double xForce, double yForce, QPen pen);
	~UniformForceGraphicsItem();
	double startX;
	double startY;
	double endX;
	double endY;
	double xForce;
	double yForce;
	double length;
	double entityDegree;
	QPen myPen;
protected:
	QRectF boundingRect() const override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

