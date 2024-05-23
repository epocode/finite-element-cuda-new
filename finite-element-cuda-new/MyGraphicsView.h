#pragma once
#include <QGraphicsView>
#include <QWidget>
#include <QWheelEvent>
#include <QPoint>
#include <QVector>
#include <QLabel>

class MyGraphicsView : public QGraphicsView
{
	Q_OBJECT;
public:
	enum Mode { COMMON, CREATELINE, CREATERECT, CREATECIRCLE};
	Mode curMode;
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

public:
	MyGraphicsView(QWidget* parent = nullptr);
	void setupDragMode();
	void setMode(Mode mode);
	void setMode(QString mode);
	void handleCoordinateInput(QString text);
	bool isCloseToFirstPoint(const QPointF& mousePos);
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
	void setTipsSignal(const QString &msg);


};

