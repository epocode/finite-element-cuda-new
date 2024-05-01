#include "MyGraphicsView.h"

MyGraphicsView::MyGraphicsView(QWidget* parent)
    : QGraphicsView(parent) {
    // 初始化代码
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setupDragMode();
    // 设置其他所需属性
}

void MyGraphicsView::wheelEvent(QWheelEvent* event) {
    const double scaleFactor = 1.15; // 缩放的比例因子
    QPoint scrollAmount = event->angleDelta();
    if (scrollAmount.y() > 0) {
        // 向上滚动鼠标滚轮，放大
        scale(scaleFactor, scaleFactor);
    }
    else {
        // 向下滚动鼠标滚轮，缩小
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }

    // 如果不需要默认的滚动行为，则不调用基类的 wheelEvent
    // QGraphicsView::wheelEvent(event);
}

void MyGraphicsView::setupDragMode()
{
    setDragMode(QGraphicsView::ScrollHandDrag);
}
