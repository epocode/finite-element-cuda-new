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
	setCurrentIndex(curState); // 确保窗口显示时设置索引
}

