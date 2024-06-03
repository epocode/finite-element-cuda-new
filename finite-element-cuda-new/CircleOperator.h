#pragma once
#include "AbstractGraphicsviewOperator.h"
#include "MygraphicsView.h"

class MyGraphicsView;

class CircleOperator :
    public AbstractGraphicsviewOperator
{
public:
    MyGraphicsView* view;
    CircleOperator(MyGraphicsView* view);
    void handleCoordinateInput(QString text);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
};

