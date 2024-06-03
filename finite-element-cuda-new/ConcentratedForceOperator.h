#pragma once
#include "AbstractGraphicsviewOperator.h"
#include "MygraphicsView.h"

class MyGraphicsView;
class ConcentratedForceOperator :
    public AbstractGraphicsviewOperator
{
public:
    MyGraphicsView* view;
    ConcentratedForceOperator(MyGraphicsView* view);
    void handleCoordinateInput(QString text);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void handleInput(double x, double y);
};

