#include "MyGraphicsView.h"

MyGraphicsView::MyGraphicsView(QWidget* parent)
    : QGraphicsView(parent) {
    // ��ʼ������
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setupDragMode();
    // ����������������
}

void MyGraphicsView::wheelEvent(QWheelEvent* event) {
    const double scaleFactor = 1.15; // ���ŵı�������
    QPoint scrollAmount = event->angleDelta();
    if (scrollAmount.y() > 0) {
        // ���Ϲ��������֣��Ŵ�
        scale(scaleFactor, scaleFactor);
    }
    else {
        // ���¹��������֣���С
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }

    // �������ҪĬ�ϵĹ�����Ϊ���򲻵��û���� wheelEvent
    // QGraphicsView::wheelEvent(event);
}

void MyGraphicsView::setupDragMode()
{
    setDragMode(QGraphicsView::ScrollHandDrag);
}
