#include "MyStackedWidget.h"
#include <QLabel>
#include <QHBoxLayout>
MyStackedWidget::MyStackedWidget(QWidget* parent)
{
	curState = INIT;
}

void MyStackedWidget::showEvent(QShowEvent* event)
{
	QStackedWidget::showEvent(event);
	setCurrentIndex(curState); // ȷ��������ʾʱ��������
}

