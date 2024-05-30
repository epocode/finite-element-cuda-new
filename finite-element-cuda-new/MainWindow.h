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
    QLabel* paramsLabel;
    QLabel* lcValueLabel;
    QString htmlTemplate = "<html><h1> 模型应力应变位移信息< / h1><h2>位置坐标< / h2>< p > x: %1, y : %2 < / p ><h2>位移信息< / h2><p>水平位移 : %3, 竖直位移 : %4 < / p <h2>应变信息< / h2<p>水平应变 : %5, 竖直应变 : %6, 切应变 : %7 < / p <h2>应力信息< / h2<p>水平应力 : %8, 竖直应力 : %9, 切应力 : %10 < / p < / html>";

    void calcMatrixConcurrent();
    void setMyStatus(double E, double v, double t);
    void setMyStatus(double meshNums, double pointNums);
    void setLcValue(double lc);
private:
    //工具栏
    QToolBar* graphicToolBar;
    QToolBar* attributeToolBar;
    QToolBar* loadToolBar;
    QToolBar* visualizeToolBar;


private:
    //状态栏信息
    double E;
    double v;
    double t;
    double lcValue;
signals:
    void updateProgressBarSignal(int value);
    void enableRenderActionSignal(bool enable);
    void sendTextToGraphicViewSignal(const QString& text);
private slots:
    void clear();
    void addPolygon();
    void addRect();
    void addCircle();
    void createPolygonMsh(QVector<QPointF> points);
    void createRectMsh(QPointF startPoint, QPointF endPoint);
    void createCircleToMsh(double x, double y, double radius);

    void textEntered();
    void resetInputArea();
    void setTips(const QString &msg);

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


