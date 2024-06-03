#pragma once
#include "AbstractGraphicsviewOperator.h"
#include "MygraphicsView.h"

class MyGraphicsView;

class CommonOperator :
    public AbstractGraphicsviewOperator
{
public:
    MyGraphicsView* view;
    CommonOperator(MyGraphicsView* view);
    void handleCoordinateInput(QString text);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
};

