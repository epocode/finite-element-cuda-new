#pragma once
#include <QString>
#include <QMouseEvent>
#include <QKeyEvent>;

class AbstractGraphicsviewOperator
{
public:
	virtual void handleCoordinateInput(QString text) = 0;
	virtual void mousePressEvent(QMouseEvent* event) = 0;
	virtual void mouseMoveEvent(QMouseEvent* event) = 0;
	virtual void keyPressEvent(QKeyEvent* event) = 0;
};

