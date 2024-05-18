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
	enum Mode { COMMON, CREATELINE };
	Mode curMode;
	bool isDragging;
	QPoint lastMousePos;
	QVector<QPointF> points;
	QGraphicsScene* myScene;
	QGraphicsLineItem* tempLine = nullptr;
	QPen pen;
	bool isSettingPosition = false;
	QLabel* coordinateLabel;

public:
	MyGraphicsView(QWidget* parent = nullptr);
	void setupDragMode();
	void setMode(Mode mode);
	void setMode(QString mode);
	bool isPolygonClosed(QPointF p1, QPointF p2);
	void handleCoordinateInput(QString text);
protected:
	void wheelEvent(QWheelEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	bool isCloseToFirstPoint(const QPointF& mousePos);
	
signals:
	void doubleClicked(QPointF point);
	void createPolygonSignal(QVector<QPointF> points);
};

