#pragma once
#include "AbstractGraphicsviewOperator.h"
#include "MyGraphicsView.h"

class MyGraphicsView;

class PolygonOperator :
    public AbstractGraphicsviewOperator
{
public:
    MyGraphicsView* view;
    PolygonOperator(MyGraphicsView* view);
    void handleCoordinateInput(QString text);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
};

