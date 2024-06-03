#pragma once

#include <QObject>

class keyFocusFixFilter  : public QObject
{
	Q_OBJECT

public:
	keyFocusFixFilter(QObject *parent);
	~keyFocusFixFilter();
protected:
	bool eventFilter(QObject* obj, QEvent* event) override;
};
