
#include "keyFocusFixFilter.h"
#include <QEvent>
#include <QLineEdit>
#include <QKeyEvent>
#include <QStackedWidget>
keyFocusFixFilter::keyFocusFixFilter(QObject *parent)
	: QObject(parent)
{}

keyFocusFixFilter::~keyFocusFixFilter()
{}

bool keyFocusFixFilter::eventFilter(QObject * obj, QEvent * event)
{
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		// 特别处理Esc键
		if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
			//QLineEdit* lineEdit = qobject_cast<QLineEdit*>(obj);
			//if (lineEdit) {
			//	lineEdit->setFocus();
			//	return true;
			//}
			QStackedWidget* widget = qobject_cast<QStackedWidget*>(obj);
			if (widget) {
				widget->setFocus();
			}
		}
	}
	//if (event->type() == QEvent::FocusOut) {
	//	QLineEdit* lineEdit = qobject_cast<QLineEdit*>(obj);
	//	if (lineEdit) {
	//		lineEdit->setFocus();
	//		return true;
	//	}
	//}
	return QObject::eventFilter(obj, event);
}
