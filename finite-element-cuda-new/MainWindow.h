#pragma once

#include <QMainWindow>
#include <QPen>
#include "publicElement.h"
#include <QLabel>
#include <QGraphicsItem>

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
    QAction *renderStressAction;
    QAction* renderUAction;
    QAction* renderVAction;
    QAction *calcAction;
    QLabel *permanentLabel;
    QLabel* paramsLabel;
    QLabel* lcValueLabel;
    QString htmlTemplate = "<html><h1> 模型应力应变位移信息</h1><h2>位置坐标</h2><p> x: %1, y : %2 </p><h2>位移信息</h2><p>水平位移 : %3, 竖直位移 : %4 </p> <h2>应变信息</h2><p>水平应变 : %5, 竖直应变 : %6, 切应变:%7 </p><p>等效应变:%8</p> <h2>应力信息</h2><p>水平应力 : %9, 竖直应力 : %10, 切应力 : %11 </p><p>等效应力:%12</p> </html>";

    void calcMatrixConcurrent();
    void setMyStatus(double E, double v, double t);
    void setMyStatus(double meshNums, double pointNums);
    void setLcValue(double lc);
public: //这里用来保存场景中的项目
    double stressEdges[2];
    double uEdges[2];
    double vEdges[2];
    QList<QGraphicsItem*> originItems;
    QList<QGraphicsItem*> stressItems;
    QList<QGraphicsItem*> uItems;
    QList<QGraphicsItem*> vItems;
    int curItemsFlag;
private:
    //工具栏
    QToolBar* graphicToolBar;
    QToolBar* mshToolBar;
    QToolBar* attributeToolBar;
    QToolBar* loadToolBar;
    QToolBar* calcToolBar;
    QToolBar* visualizeToolBar;
    enum ToolBarStatus{GRAPH, MSH,ATTRIB, LOAD, CALC, VISUALIZE};
    ToolBarStatus curStatus = GRAPH;
    QVector<QToolBar*> toolBarList;
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
public:
    void setToolBarStatus(ToolBarStatus status);
    void generateAllColorMap();
    void clearItems();
private slots:
    void onComboBoxIndexChanged(int index);
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
    void generateMsh();
    void paintMsh();
    void setElasticAttrib();
    void setThicknessAtrrib();
    void addConcentratedForce();
    void addUniformForce();
    void addEdges();
    void calcMatrix();
    void renderStress();
    void renderU();
    void renderV();
    void saveMsh();
    void openMsh();
    void showInfoFromSignal(QString msg);
    void saveConstraint();
    void openConstraint();
    void handleDoubleClick(QPointF point);
    void activateCalc();
    void setRenderEnable(bool enable);
    void updateProgressBar(int value);
    void showConcentratedForceInfo(double x, double y, double xForce, double yForce);
    void showUniformForceInfo(double startX, double startY, double endX, double endY, double xForce, double yForce, vector<Force> forces);

};


