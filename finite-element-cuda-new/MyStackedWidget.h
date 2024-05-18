#pragma once
#include <QStackedWidget>
class MyStackedWidget :
    public QStackedWidget
{
    Q_OBJECT;
public:
    enum State{INIT, START, END};
    State curState;
public:
    MyStackedWidget(QWidget* parent = nullptr);
protected:
    void showEvent(QShowEvent* event) override;
};

