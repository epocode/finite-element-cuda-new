#include "MyGradientLabel.h"
#include <QPainter>
#include <QGraphicsView>
MyGradientLabel::MyGradientLabel(QWidget *parent = nullptr)
	: QLabel(parent)
{
	setFixedSize(30, 150);
}

MyGradientLabel::~MyGradientLabel()
{}

void MyGradientLabel::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    QLinearGradient linearGradient(rect().topLeft(), rect().bottomLeft());

    // ���ý������ɫ��λ��
    linearGradient.setColorAt(0, Qt::red);    // ���俪ʼ������ɫ
    linearGradient.setColorAt(0.5, Qt::yellow); // �м�λ�õ���ɫ
    linearGradient.setColorAt(1, Qt::green);  // �������������ɫ

    // ʹ�ý���ɫ��Ϊ��ˢ���
    painter.fillRect(rect(), linearGradient);
}
