#include "MyStackedWidget.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>
MyStackedWidget::MyStackedWidget(QWidget* parent)
	: QStackedWidget(parent)
{
	curMode = INIT;
	this->focusFilter = new keyFocusFixFilter(this);
	//this->installEventFilter(focusFilter);
}

void MyStackedWidget::setMode(Mode mode)
{
	this->curMode = mode;
	setCurrentIndex(curMode);
}

void MyStackedWidget::showEvent(QShowEvent* event)
{
	QStackedWidget::showEvent(event);
	setCurrentIndex(curMode); // 确保窗口显示时设置索引
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

void MyStackedWidget::setCurrentIndex(int index)
{
	//QWidget* currentWidget = this->currentWidget();
	//if (currentWidget) {
	//	QLineEdit* editLine = currentWidget->findChild<QLineEdit*>();
	//	if (editLine && focusFilter) {
	//		editLine->removeEventFilter(focusFilter);
	//		delete focusFilter;
	//	}
	//}
	QStackedWidget::setCurrentIndex(index);
	QWidget* currentWidget = this->currentWidget();
	//if (currentWidget) {
	//	QLineEdit* editLine = currentWidget->findChild<QLineEdit*>();
	//	if (editLine) {
	//		this->focusFilter = new keyFocusFixFilter(editLine);
	//		editLine->installEventFilter(focusFilter);
	//		editLine->setFocusPolicy(Qt::StrongFocus);
	//		editLine->setFocus();
	//	}
	//}
}

