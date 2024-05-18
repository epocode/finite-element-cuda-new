#include "MyStackedWidget.h"
#include <QLabel>
#include <QHBoxLayout>
MyStackedWidget::MyStackedWidget(QWidget* parent)
	: QStackedWidget(parent)
{
	curMode = INIT;
}

void MyStackedWidget::setMode(Mode mode)
{
	this->curMode = mode;
	setCurrentIndex(curMode);
}

void MyStackedWidget::showEvent(QShowEvent* event)
{
	QStackedWidget::showEvent(event);
	setCurrentIndex(curMode); // ȷ��������ʾʱ��������
}

void MyStackedWidget::setMode(QString mode)
{
	if (mode == QString("INIT")) {
		setMode(INIT);
	}
	else if (mode == QString("START")) {
		setMode(START);
	}
	else if (mode == QString("END")) {
		setMode(END);
	}
}

