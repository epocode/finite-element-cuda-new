#include "MyGradientLabel.h"
#include <QPainter>
#include <QGraphicsView>
MyGradientLabel::MyGradientLabel(QWidget *parent = nullptr)
	: QLabel(parent)
{
	setFixedSize(40, 150);
}

MyGradientLabel::~MyGradientLabel()
{}

void MyGradientLabel::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    QLinearGradient linearGradient(rect().topLeft(), rect().bottomLeft());

    // 设置渐变的颜色和位置
    linearGradient.setColorAt(0, Qt::red);    // 渐变开始处的颜色
    linearGradient.setColorAt(0.33, Qt::yellow); // 中间位置的颜色
    linearGradient.setColorAt(0.66, Qt::green);  // 渐变结束处的颜色
    linearGradient.setColorAt(1, Qt::blue);

    // 使用渐变色作为画刷填充
    painter.fillRect(rect(), linearGradient);
}
