#pragma once
#include "AbstractGraphicsviewOperator.h"
#include "MygraphicsView.h"

class MyGraphicsView;
class AddConstraintOperator :
    public AbstractGraphicsviewOperator
{
public:
    MyGraphicsView* view;
    AddConstraintOperator(MyGraphicsView* view);
    void handleCoordinateInput(QString text);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void handleInput(QPointF point);
};

