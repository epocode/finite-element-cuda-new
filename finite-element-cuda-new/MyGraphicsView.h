#pragma once
#include <QGraphicsView>
#include <QWidget>
#include <QWheelEvent>
#include <QPoint>
#include <QVector>
#include <QLabel>
#include "MyGradientLabel.h"
#include <QGroupBox>
#include "AbstractGraphicsviewOperator.h"
#include "PolygonOperator.h"
#include "CommonOperator.h"
#include "RectOperator.h"
#include "CircleOperator.h"
#include "publicElement.h"
class MyGraphicsView : public QGraphicsView
{
	Q_OBJECT;
public:
	enum Mode { COMMON, CREATELINE, CREATERECT, CREATECIRCLE, CONCENTRATEDFORCE, UNIFORMFORCE, CONSTRAINT};
	bool isDragging;
	QPoint lastMousePos;
	QVector<QPointF> points;
	QGraphicsScene* myScene;
	QGraphicsLineItem* tempLine = nullptr;
	QGraphicsEllipseItem* tempEllipse = nullptr;
	QPen pen;
	bool isSettingPosition = false;
	QLabel* coordinateLabel;
	QPointF rectStartPoint;
	QPointF startPoint;
	QGraphicsRectItem* tempRect = nullptr;
	bool isDrawing;
	QVector<QGraphicsLineItem*> lineItems;
	QVector<QGraphicsEllipseItem*> ellipseItemList;
	MyGradientLabel* colorBar;
	QLabel* maxGradientLabel;
	QLabel* minGradientLabel;
	QGroupBox* gradientBox;
	AbstractGraphicsviewOperator* myOperator;
	QVector<AbstractGraphicsviewOperator*> operatorList;
public:
	MyGraphicsView(QWidget* parent = nullptr);
	void setupDragMode();
	void setMode(Mode mode);
	void setMode(QString mode);
	void handleCoordinateInput(QString text);
	bool isCloseToFirstPoint(const QPointF& mousePos);
	void hideRenderInfo();
	void showRenderInfo(double max, double min);
	void handleDirectForceInput(Force force);
	void handleDirectConstraintInput(EdgeInfo edgeInfo);
protected:
	void wheelEvent(QWheelEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void keyPressEvent(QKeyEvent* event)override;
	
signals:
	void doubleClicked(QPointF point);
	void createPolygonSignal(QVector<QPointF> points);
	void createRectSignal(QPointF startPoint, QPointF endPoint);
	void createCircleSignal(double x, double y, double radius);
	void resetInputAreaSignal();
	void setTipsSignal(const QString &msg);//设置输入框的提示
	void addConcentratedForceSignal(double x, double y, double xForce, double yForce);
	void addUniformForceSignal(double startX, double startY, double endX, double endY, double xForce, double yForce);
	void addConstraintSignal(QString msg);
	void sendCalcActivate();
};

