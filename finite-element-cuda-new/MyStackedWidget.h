#pragma once
#include <QStackedWidget>

namespace Ui {
    class MyStackedWidget;
}
class MyStackedWidget :
    public QStackedWidget
{
    Q_OBJECT;
public:
    enum Mode{INIT, START, END};
    Mode curMode;

public:
    MyStackedWidget(QWidget* parent = nullptr);
    void setMode(Mode mode);
    void setMode(QString mode);
protected:
    void showEvent(QShowEvent* event) override;
};

