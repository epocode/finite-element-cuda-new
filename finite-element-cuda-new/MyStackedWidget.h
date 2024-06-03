#pragma once
#include <QStackedWidget>
#include "keyFocusFixFilter.h"
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
    keyFocusFixFilter* focusFilter;
public:
    MyStackedWidget(QWidget* parent = nullptr);
    void setMode(Mode mode);
    void setMode(QString mode);
    void setCurrentIndex(int index);
protected:
    void showEvent(QShowEvent* event) override;
};

