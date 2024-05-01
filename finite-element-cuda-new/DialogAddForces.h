#pragma once

#include <QDialog>

namespace Ui {
class DialogAddForces;
}

class DialogAddForces : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddForces(QWidget *parent = nullptr);
    ~DialogAddForces();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::DialogAddForces *ui;
};
