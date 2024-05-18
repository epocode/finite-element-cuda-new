#pragma once

#include <QDialog>
#include "publicElement.h"
namespace Ui {
class DialogAddGraphics;
}

class DialogAddGraphics : public QDialog
{
    Q_OBJECT

private:
    double lc;
public:
    explicit DialogAddGraphics(QWidget *parent = nullptr, double lc=0.0);
    ~DialogAddGraphics();

private slots:
    void on_addPointButton_clicked();

    void on_pushButton_clicked();

signals:
    void sendRectSignal(Rect rect);
    void sendCircleSignal(Circle circle);

private:
    Ui::DialogAddGraphics *ui;
};
