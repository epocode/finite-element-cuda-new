#pragma once
#include "AbstractGraphicsviewOperator.h"
#include "MygraphicsView.h"

class MyGraphicsView;
class UniformForceOperator :
    public AbstractGraphicsviewOperator
{
public:
    MyGraphicsView* view;
    bool isFirst;
    double startX;
    double startY;
    QGraphicsEllipseItem* tempItem;
    UniformForceOperator(MyGraphicsView* view);
    void handleCoordinateInput(QString text);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void handleInput(double x, double y);
};

