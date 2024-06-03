#pragma once
#include "AbstractGraphicsviewOperator.h"
#include "MygraphicsView.h"

class MyGraphicsView;
class RectOperator :
    public AbstractGraphicsviewOperator
{
public:
    MyGraphicsView* view;
    RectOperator(MyGraphicsView* view);
    void handleCoordinateInput(QString text);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
};

