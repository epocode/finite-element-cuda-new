#pragma once
#include <QGraphicsView>
#include <QWidget>
#include <QWheelEvent>
class MyGraphicsView : public QGraphicsView
{
	Q_OBJECT;
public:
	MyGraphicsView(QWidget* parent = nullptr);
	void setupDragMode();
protected:
	void wheelEvent(QWheelEvent* event) override;
};

