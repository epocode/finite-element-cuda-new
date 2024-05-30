#pragma once

#include <QObject>
#include <QLabel>
class MyGradientLabel  : public QLabel
{
	Q_OBJECT

public:
	MyGradientLabel(QWidget *parent);
	~MyGradientLabel();
protected:
	void paintEvent(QPaintEvent* event) override;
};
