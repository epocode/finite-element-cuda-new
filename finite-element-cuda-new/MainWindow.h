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
    struct pair_hash {
        inline std::size_t operator()(const std::pair<int, int>& v) const {
            return v.first * 31 + v.second;
        }
    };
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
    void handleDoubleClick(QPointF point);
    void activateCalc();
    void setRenderEnable(bool enable);
    void updateProgressBar(int value);
};


