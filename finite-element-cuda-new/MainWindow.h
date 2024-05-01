#pragma once

#include <QMainWindow>
#include <QPen>
#include "publicElement.h"
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QPen pen;
    bool paintState;

private:
    Ui::MainWindow *ui;
    QAction *renderAction;
    QAction *calcAction;
    QLabel *permanentLabel;

    void calcMatrixConcurrent();

signals:
    void updateProgressBarSignal(int value);
    void enableRenderActionSignal(bool enable);

private slots:
    void clear();
    void addGraphics();
    void paintRect(Rect rect);
    void paintCircle(Circle circle);
    void generateMsh();
    void paintMsh();
    void addForces();
    void addEdges();
    void calcMatrix();
    void render();
    void saveMsh();
    void openMsh();
    void saveConstraint();
    void openConstraint();
    void handleClick(QPointF point);
    void activateCalc();
    void setRenderEnable(bool enable);
    void updateProgressBar(int value);
};


