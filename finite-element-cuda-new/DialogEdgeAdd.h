#pragma once

#include <QDialog>

namespace Ui {
class DialogEdgeAdd;
}

class DialogEdgeAdd : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEdgeAdd(QWidget *parent = nullptr);
    ~DialogEdgeAdd();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DialogEdgeAdd *ui;
signals:
    void sendCalcActivate();
};
